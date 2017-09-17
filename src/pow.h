// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <stdint.h>

class arith_uint256;
class uint256;
class CBlockHeader;
class CBlockIndex;

#define PERCENT_FACTOR 100

#define BLOCK_TYPE CBlockHeader *
#define BLOCK_TIME(block) block->nTime
#define INDEX_TYPE CBlockIndex *
#define INDEX_HEIGHT(block) block->nHeight
#define INDEX_TIME(block) block->GetBlockTime()
#define INDEX_PREV(block) block->pprev
#define INDEX_TARGET(block) block->nBits
#define DIFF_SWITCHOVER(TEST, MAIN) (GetBoolArg("-testnet", false) ? TEST : MAIN)
#define DIFF_ABS std::abs
#define SET_COMPACT(EXPANDED, COMPACT) EXPANDED.SetCompact(COMPACT)
#define GET_COMPACT(EXPANDED) EXPANDED.GetCompact()
#define BIGINT_MULTIPLY(x, y) x* y
#define BIGINT_DIVIDE(x, y) x / y
#define BIGINT_GREATER_THAN(x, y) (x > y)

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, bool IsProofOfStake = false);
unsigned int PoSDifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast);
unsigned int ZenDifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast);
unsigned int DeltaDifficultyAdjumentAlgorithm(const INDEX_TYPE pindexLast, const BLOCK_TYPE block);
unsigned int DGW3DifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast, const CBlockHeader* pblock);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits);
uint256 GetBlockProof(const CBlockIndex& block);

#endif // BITCOIN_POW_H
