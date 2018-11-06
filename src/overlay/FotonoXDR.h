#pragma once
#include "xdr/fotono-ledger-entries.h"
#include "xdr/fotono-ledger.h"
#include "xdr/fotono-overlay.h"
#include "xdr/fotono-transaction.h"
#include "xdr/fotono-types.h"

namespace fotono
{

std::string xdr_printer(const PublicKey& pk);
}
