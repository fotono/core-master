#pragma once

// Copyright 2014 fotono Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0
#include "util/asio.h"

#include "history/HistoryManager.h"
#include "ledger/LedgerHeaderFrame.h"
#include "ledger/LedgerManager.h"
#include "ledger/SyncingLedgerChain.h"
#include "main/PersistentState.h"
#include "transactions/TransactionFrame.h"
#include "util/Timer.h"
#include "xdr/fotono-ledger.h"
#include <string>

/*
Holds the current ledger
Applies the tx set to the last ledger to get the next one
Hands the old ledger off to the history
*/

namespace medida
{
class Timer;
class Counter;
class Histogram;
}

namespace fotono
{
class Application;
class Database;
class LedgerDelta;

class LedgerManagerImpl : public LedgerManager
{
    LedgerHeaderHistoryEntry mLastClosedLedger;
    LedgerHeaderFrame::pointer mCurrentLedger;

    Application& mApp;
    medida::Timer& mTransactionApply;
    medida::Histogram& mTransactionCount;
    medida::Timer& mLedgerClose;
    medida::Timer& mLedgerAgeClosed;
    medida::Counter& mLedgerAge;
    medida::Counter& mLedgerStateCurrent;
    medida::Timer& mLedgerStateChanges;
    VirtualClock::time_point mLastClose;
    VirtualClock::time_point mLastStateChange;

    medida::Counter& mSyncingLedgersSize;

    SyncingLedgerChain mSyncingLedgers;
    uint32_t mCatchupTriggerLedger{0};

    CatchupState mCatchupState{CatchupState::NONE};

    void initializeCatchup(LedgerCloseData const& ledgerData);
    void continueCatchup(LedgerCloseData const& ledgerData);
    void finalizeCatchup(LedgerCloseData const& ledgerData);

    void addToSyncingLedgers(LedgerCloseData const& ledgerData);
    void startCatchupIf(uint32_t lastReceivedLedgerSeq);

    void historyCaughtup(asio::error_code const& ec,
                         CatchupWork::ProgressState progressState,
                         LedgerHeaderHistoryEntry const& lastClosed);

    void processFeesSeqNums(std::vector<TransactionFramePtr>& txs,
                            LedgerDelta& delta);
    void applyTransactions(std::vector<TransactionFramePtr>& txs,
                           LedgerDelta& ledgerDelta,
                           TransactionResultSet& txResultSet);

    void ledgerClosed(LedgerDelta const& delta);
    void storeCurrentLedger();
    void advanceLedgerPointers();

    enum class CloseLedgerIfResult
    {
        CLOSED,
        TOO_OLD,
        TOO_NEW
    };
    CloseLedgerIfResult closeLedgerIf(LedgerCloseData const& ledgerData);

    State mState;
    void setState(State s);
    void setCatchupState(CatchupState s);

  public:
    LedgerManagerImpl(Application& app);

    void bootstrap() override;
    State getState() const override;
    CatchupState getCatchupState() const override;
    std::string getStateHuman() const override;

    void valueExternalized(LedgerCloseData const& ledgerData) override;

    uint32_t getLedgerNum() const override;
    uint32_t getLastClosedLedgerNum() const override;
    int64_t getMinBalance(uint32_t ownerCount) const override;
    uint32_t getTxFee() const override;
    uint32_t getMaxTxSetSize() const override;
    uint64_t getCloseTime() const override;
    uint64_t secondsSinceLastLedgerClose() const override;
    void syncMetrics() override;

    void startNewLedger(LedgerHeader genesisLedger);
    void startNewLedger() override;
    void loadLastKnownLedger(
        std::function<void(asio::error_code const& ec)> handler) override;

    LedgerHeaderHistoryEntry const& getLastClosedLedgerHeader() const override;
    LedgerHeader const& getCurrentLedgerHeader() const override;
    LedgerHeader& getCurrentLedgerHeader() override;
    uint32_t getCurrentLedgerVersion() const override;

    Database& getDatabase() override;

    void startCatchup(CatchupConfiguration configuration,
                      bool manualCatchup) override;

    HistoryManager::LedgerVerificationStatus
    verifyCatchupCandidate(LedgerHeaderHistoryEntry const&,
                           bool manualCatchup) const override;
    void closeLedger(LedgerCloseData const& ledgerData) override;
    void deleteOldEntries(Database& db, uint32_t ledgerSeq,
                          uint32_t count) override;
    void checkDbState() override;
};
}