#pragma once

// Copyright 2016 fotono Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include <bitset>
#include <memory>
#include <vector>

namespace fotono
{

// Abstract enumerator type for sets-of-bitsets.
class BitsetEnumerator
{
  public:
    virtual void reset() = 0;
    virtual void operator++() = 0;
    virtual std::bitset<64> operator*() const = 0;
    virtual operator bool() const = 0;
};

// Enumerates a single bitset once, then stops.
class ConstantEnumerator : public BitsetEnumerator
{
    std::bitset<64> const mBits;
    bool mDone;

  public:
    ConstantEnumerator(std::bitset<64> bits);
    void reset() override;
    operator bool() const override;
    std::bitset<64> operator*() const override;
    void operator++() override;

    static std::shared_ptr<BitsetEnumerator> bitNumber(size_t n);
    static std::vector<std::shared_ptr<BitsetEnumerator>>
    bitNumbers(std::vector<size_t> ns);
};

/*
 * Enumerates the permutations of N bits of T total (<= 64) bits.
 *
 * (That is, it returns "T choose N" bitsets, each with N 1-bits)
 *
 * For example, if configured to emit 4 of 6 bits, it'll output:
 *
 * 0000000000000000000000000000000000000000000000000000000000001111
 * 0000000000000000000000000000000000000000000000000000000000010111
 * 0000000000000000000000000000000000000000000000000000000000011011
 * 0000000000000000000000000000000000000000000000000000000000011101
 * 0000000000000000000000000000000000000000000000000000000000011110
 * 0000000000000000000000000000000000000000000000000000000000100111
 * 0000000000000000000000000000000000000000000000000000000000101011
 * 0000000000000000000000000000000000000000000000000000000000101101
 * 0000000000000000000000000000000000000000000000000000000000101110
 * 0000000000000000000000000000000000000000000000000000000000110011
 * 0000000000000000000000000000000000000000000000000000000000110101
 * 0000000000000000000000000000000000000000000000000000000000110110
 * 0000000000000000000000000000000000000000000000000000000000111001
 * 0000000000000000000000000000000000000000000000000000000000111010
 * 0000000000000000000000000000000000000000000000000000000000111100
 */

class PermutationEnumerator : public BitsetEnumerator
{
    uint64_t mCur;     // Current permutation of bits.
    size_t const mSet; // Number of bits that should be set.
    size_t const mTot; // Number of bits to select from.
  public:
    PermutationEnumerator(size_t nSet, size_t nTotal);
    void reset() override;
    operator bool() const override;
    std::bitset<64> operator*() const override;
    void operator++() override;
};

/*
 * Enumerates the nonempty powerset of a number of bits. This is
 * just the set of numbers from 1 to (2<<nBits)-1 inclusive.
 */

class PowersetEnumerator : public BitsetEnumerator
{
    uint64_t mCur;       // Current permutation of bits.
    uint64_t const mLim; // One past the last set to emit.
  public:
    PowersetEnumerator(size_t nBits);
    void reset() override;
    operator bool() const override;
    std::bitset<64> operator*() const override;
    void operator++() override;
};

/*
 * Enumerates the cartesion product of N enumerators, OR-ing together the
 * bitsets returned from each, for each call.
 *
 * For example, if configured with two constant enumerators for bits 1 and
 * 3, will enumerate a single entry:
 *
 * 0000000000000000000000000000000000000000000000000000000000001010
 *
 * Alternatively if configured with two selection enumerators (see below)
 * 1-of-{0,1} and 1-of-{4,5}, will enumerate:
 *
 * 0000000000000000000000000000000000000000000000000000000011000011
 * 0000000000000000000000000000000000000000000000000000000011000101
 * 0000000000000000000000000000000000000000000000000000000011000110
 * 0000000000000000000000000000000000000000000000000000000101000011
 * 0000000000000000000000000000000000000000000000000000000101000101
 * 0000000000000000000000000000000000000000000000000000000101000110
 * 0000000000000000000000000000000000000000000000000000000110000011
 * 0000000000000000000000000000000000000000000000000000000110000101
 * 0000000000000000000000000000000000000000000000000000000110000110
 *
 */
class CartesianProductEnumerator : public BitsetEnumerator
{
    std::vector<std::shared_ptr<BitsetEnumerator>> mInnerEnums;

  public:
    CartesianProductEnumerator(
        std::vector<std::shared_ptr<BitsetEnumerator>> innerEnums);
    void reset() override;
    operator bool() const override;
    std::bitset<64> operator*() const override;
    void operator++() override;
};

/*
 * Uses an "index" BitsetEnumerator to repeatedly select subsets of
 * a set of user-provided inner enumerators, forms a cartesian product
 * enumerator over each subset, and enumerates those cartesian products.
 *
 * For example, if constructed with 6 inner enumerators, and told to take
 * 4-element subsets, the auxiliary index enumerator is configured to
 * emit 4-of-6 bit permutations, as such:
 *
 * 0000000000000000000000000000000000000000000000000000000000001111
 * 0000000000000000000000000000000000000000000000000000000000010111
 * 0000000000000000000000000000000000000000000000000000000000011011
 * 0000000000000000000000000000000000000000000000000000000000011101
 * 0000000000000000000000000000000000000000000000000000000000011110
 * 0000000000000000000000000000000000000000000000000000000000100111
 * 0000000000000000000000000000000000000000000000000000000000101011
 * 0000000000000000000000000000000000000000000000000000000000101101
 * 0000000000000000000000000000000000000000000000000000000000101110
 * 0000000000000000000000000000000000000000000000000000000000110011
 * 0000000000000000000000000000000000000000000000000000000000110101
 * 0000000000000000000000000000000000000000000000000000000000110110
 * 0000000000000000000000000000000000000000000000000000000000111001
 * 0000000000000000000000000000000000000000000000000000000000111010
 * 0000000000000000000000000000000000000000000000000000000000111100
 *
 * If the set of provided inner enumerators are (say) ConstantEnumerators
 * over the bit numbers {0, 8, 0x10, 0x18, 0x20, 0x28}, then the composite
 * SelectionEnumerator will selectively emit 4-enumerator combinations,
 * such as:
 *
 * 0000000000000000000000000000000000000001000000010000000100000001
 * 0000000000000000000000000000000100000000000000010000000100000001
 * 0000000000000000000000000000000100000001000000000000000100000001
 * 0000000000000000000000000000000100000001000000010000000000000001
 * 0000000000000000000000000000000100000001000000010000000100000000
 * 0000000000000000000000010000000000000000000000010000000100000001
 * 0000000000000000000000010000000000000001000000000000000100000001
 * 0000000000000000000000010000000000000001000000010000000000000001
 * 0000000000000000000000010000000000000001000000010000000100000000
 * 0000000000000000000000010000000100000000000000000000000100000001
 * 0000000000000000000000010000000100000000000000010000000000000001
 * 0000000000000000000000010000000100000000000000010000000100000000
 * 0000000000000000000000010000000100000001000000000000000000000001
 * 0000000000000000000000010000000100000001000000000000000100000000
 * 0000000000000000000000010000000100000001000000010000000000000000
 *
 * The inner enumerators need not, of course, be constants: selective
 * enumeration can nest, as in this case of taking 2-of-3 enumerators,
 * each of which is itself a 2-of-3 constant enumerator:
 *
 * 0000000000000000000000000000000000000000000000110000000000000011
 * 0000000000000000000000000000000000000000000000110000000000000101
 * 0000000000000000000000000000000000000000000000110000000000000110
 * 0000000000000000000000000000000000000000000001010000000000000011
 * 0000000000000000000000000000000000000000000001010000000000000101
 * 0000000000000000000000000000000000000000000001010000000000000110
 * 0000000000000000000000000000000000000000000001100000000000000011
 * 0000000000000000000000000000000000000000000001100000000000000101
 * 0000000000000000000000000000000000000000000001100000000000000110
 * 0000000000000000000000000000001100000000000000000000000000000011
 * 0000000000000000000000000000001100000000000000000000000000000101
 * 0000000000000000000000000000001100000000000000000000000000000110
 * 0000000000000000000000000000010100000000000000000000000000000011
 * 0000000000000000000000000000010100000000000000000000000000000101
 * 0000000000000000000000000000010100000000000000000000000000000110
 * 0000000000000000000000000000011000000000000000000000000000000011
 * 0000000000000000000000000000011000000000000000000000000000000101
 * 0000000000000000000000000000011000000000000000000000000000000110
 * 0000000000000000000000000000001100000000000000110000000000000000
 * 0000000000000000000000000000001100000000000001010000000000000000
 * 0000000000000000000000000000001100000000000001100000000000000000
 * 0000000000000000000000000000010100000000000000110000000000000000
 * 0000000000000000000000000000010100000000000001010000000000000000
 * 0000000000000000000000000000010100000000000001100000000000000000
 * 0000000000000000000000000000011000000000000000110000000000000000
 * 0000000000000000000000000000011000000000000001010000000000000000
 * 0000000000000000000000000000011000000000000001100000000000000000
 */

class SelectionEnumerator : public BitsetEnumerator
{
    std::vector<std::shared_ptr<BitsetEnumerator>> const mInnerEnums;
    std::shared_ptr<BitsetEnumerator> mIndexEnum;
    CartesianProductEnumerator mProduct;

    static CartesianProductEnumerator
    select(std::shared_ptr<BitsetEnumerator> index,
           std::vector<std::shared_ptr<BitsetEnumerator>> const& from);

  public:
    SelectionEnumerator(
        std::shared_ptr<BitsetEnumerator> index,
        std::vector<std::shared_ptr<BitsetEnumerator>> const& innerEnums);
    std::bitset<64> operator*() const override;
    void reset() override;
    operator bool() const override;
    void operator++() override;

    static std::shared_ptr<BitsetEnumerator> bitNumbers(size_t nSel,
                                                        std::vector<size_t> ns);
};
}
