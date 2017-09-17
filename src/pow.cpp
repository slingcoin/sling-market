// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2017 The Dash Core developers
// Copyright (c) 2015-2017 The Sling developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "main.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

#include <math.h>


//unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, bool IsProofOfStake)
{
    if (IsProofOfStake) {

        return PoSDifficultyAdjumentAlgorithm(pindexLast);
    }
    else
    {
        //return DGW3DifficultyAdjumentAlgorithm(pindexLast, pblock);
        //return ZenDifficultyAdjumentAlgorithm(pindexLast);
        return DeltaDifficultyAdjumentAlgorithm(pindexLast, pblock);
    }
}

unsigned int PoSDifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast)
{
    //LogPrintf("GetNextWorkRequired: IsProofOfStake\n");
    // This is a PoS Block, calculate difficulty differently than PoW block
    uint256 bnTargetLimit = (~uint256(0) >> 24);
    int64_t nTargetTimespan = Params().PoSTargetTimespan();
    int64_t nTargetSpacing = Params().PoSTargetSpacing();

    int64_t nActualSpacing = 0;
    if (pindexLast->nHeight != 0)
        nActualSpacing = pindexLast->GetBlockTime() - pindexLast->pprev->GetBlockTime();

    if (nActualSpacing < 0)
        nActualSpacing = 1;

    // ppcoin: target change every block
    // ppcoin: retarget with exponential moving toward target spacing
    uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);

    int64_t nInterval = nTargetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;

    return bnNew.GetCompact();
}

unsigned int DeltaDifficultyAdjumentAlgorithm(const INDEX_TYPE pindexLast, const BLOCK_TYPE block)
{
    unsigned int initalBlock = 1;
    int64_t nRetargetTimespan = Params().PoWTargetSpacing();
    const unsigned int nProofOfWorkLimit = UintToArith256(Params().PoWLimit()).GetCompact();
    const unsigned int nLastBlock = 1;
    const unsigned int nShortFrame = 3;
    const unsigned int nMiddleFrame = 24;
    const unsigned int nLongFrame = 576;

    const int64_t nLBWeight = 64;
    const int64_t nShortWeight = 8;
    int64_t nMiddleWeight = 2;
    int64_t nLongWeight = 1;

    const int64_t nLBMinGap = nRetargetTimespan / 6;
    const int64_t nLBMaxGap = nRetargetTimespan * 6;

    const int64_t nQBFrame = nShortFrame + 1;
    const int64_t nQBMinGap = (nRetargetTimespan * PERCENT_FACTOR / 120) * nQBFrame;

    const int64_t nBadTimeLimit = 0;
    const int64_t nBadTimeReplace = nRetargetTimespan / 10;

    const int64_t nLowTimeLimit = nRetargetTimespan * 90 / PERCENT_FACTOR;
    const int64_t nFloorTimeLimit = nRetargetTimespan * 65 / PERCENT_FACTOR;

    const int64_t nDrift = 1;
    int64_t nLongTimeLimit = ((6 * nDrift)) * 60;
    int64_t nLongTimeStep = nDrift * 60;

    unsigned int nMinimumAdjustLimit = (unsigned int)nRetargetTimespan * 75 / PERCENT_FACTOR;
    unsigned int nMaximumAdjustLimit = (unsigned int)nRetargetTimespan * 150 / PERCENT_FACTOR;

    int64_t nDeltaTimespan = 0;
    int64_t nLBTimespan = 0;
    int64_t nShortTimespan = 0;
    int64_t nMiddleTimespan = 0;
    int64_t nLongTimespan = 0;
    int64_t nQBTimespan = 0;

    int64_t nWeightedSum = 0;
    int64_t nWeightedDiv = 0;
    int64_t nWeightedTimespan = 0;

    const INDEX_TYPE pindexFirst = pindexLast; // multi algo - last block is selected on a per algo basis.

    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    if (INDEX_HEIGHT(pindexLast) <= nQBFrame)
        return nProofOfWorkLimit;

    pindexFirst = INDEX_PREV(pindexLast);
    nLBTimespan = INDEX_TIME(pindexLast) - INDEX_TIME(pindexFirst);

    if (nLBTimespan > nBadTimeLimit && nLBTimespan < nLBMinGap)
        nLBTimespan = nLBTimespan * 50 / PERCENT_FACTOR;

    if (nLBTimespan <= nBadTimeLimit)
        nLBTimespan = nBadTimeReplace;

    if (nLBTimespan > nLBMaxGap)
        nLBTimespan = nLBTimespan * 150 / PERCENT_FACTOR;

    pindexFirst = pindexLast;
    for (unsigned int i = 1; pindexFirst != NULL && i <= nQBFrame; i++) {
        nDeltaTimespan = INDEX_TIME(pindexFirst) - INDEX_TIME(INDEX_PREV(pindexFirst));

        if (nDeltaTimespan <= nBadTimeLimit)
            nDeltaTimespan = nBadTimeReplace;

        if (i <= nShortFrame)
            nShortTimespan += nDeltaTimespan;
        nQBTimespan += nDeltaTimespan;
        pindexFirst = INDEX_PREV(pindexFirst);
    }

    if (INDEX_HEIGHT(pindexLast) - initalBlock <= nMiddleFrame) {
        nMiddleWeight = nMiddleTimespan = 0;
    }
    else {
        pindexFirst = pindexLast;
        for (unsigned int i = 1; pindexFirst != NULL && i <= nMiddleFrame; i++) {
            nDeltaTimespan = INDEX_TIME(pindexFirst) - INDEX_TIME(INDEX_PREV(pindexFirst));

            if (nDeltaTimespan <= nBadTimeLimit)
                nDeltaTimespan = nBadTimeReplace;

            nMiddleTimespan += nDeltaTimespan;
            pindexFirst = INDEX_PREV(pindexFirst);
        }
    }

    if (INDEX_HEIGHT(pindexLast) - initalBlock <= nLongFrame) {
        nLongWeight = nLongTimespan = 0;
    }
    else {
        pindexFirst = pindexLast;
        for (unsigned int i = 1; pindexFirst != NULL && i <= nLongFrame; i++)
            pindexFirst = INDEX_PREV(pindexFirst);

        nLongTimespan = INDEX_TIME(pindexLast) - INDEX_TIME(pindexFirst);
    }

    if ((nQBTimespan > nBadTimeLimit) && (nQBTimespan < nQBMinGap) && (nLBTimespan < nRetargetTimespan * 40 / PERCENT_FACTOR)) {
        nMiddleWeight = nMiddleTimespan = nLongWeight = nLongTimespan = 0;
    }

    nWeightedSum = (nLBTimespan * nLBWeight) + (nShortTimespan * nShortWeight);
    nWeightedSum += (nMiddleTimespan * nMiddleWeight) + (nLongTimespan * nLongWeight);
    nWeightedDiv = (nLastBlock * nLBWeight) + (nShortFrame * nShortWeight);
    nWeightedDiv += (nMiddleFrame * nMiddleWeight) + (nLongFrame * nLongWeight);
    nWeightedTimespan = nWeightedSum / nWeightedDiv;

    if (DIFF_ABS(nLBTimespan - nRetargetTimespan) < nRetargetTimespan * 20 / PERCENT_FACTOR) {
        nMinimumAdjustLimit = (unsigned int)nRetargetTimespan * 90 / PERCENT_FACTOR;
        nMaximumAdjustLimit = (unsigned int)nRetargetTimespan * 110 / PERCENT_FACTOR;
    } else if (DIFF_ABS(nLBTimespan - nRetargetTimespan) < nRetargetTimespan * 30 / PERCENT_FACTOR) {
        nMinimumAdjustLimit = (unsigned int)nRetargetTimespan * 80 / PERCENT_FACTOR;
        nMaximumAdjustLimit = (unsigned int)nRetargetTimespan * 120 / PERCENT_FACTOR;
    }

    if (nWeightedTimespan < nMinimumAdjustLimit)
        nWeightedTimespan = nMinimumAdjustLimit;

    if (nWeightedTimespan > nMaximumAdjustLimit)
        nWeightedTimespan = nMaximumAdjustLimit;

    arith_uint256 bnNew;
    SET_COMPACT(bnNew, INDEX_TARGET(pindexLast));
    bnNew = BIGINT_MULTIPLY(bnNew, arith_uint256(nWeightedTimespan));
    bnNew = BIGINT_DIVIDE(bnNew, arith_uint256(nRetargetTimespan));

    nLBTimespan = INDEX_TIME(pindexLast) - INDEX_TIME(INDEX_PREV(pindexLast));
    arith_uint256 bnComp;
    SET_COMPACT(bnComp, INDEX_TARGET(pindexLast));
    if (nLBTimespan > 0 && nLBTimespan < nLowTimeLimit && BIGINT_GREATER_THAN(bnNew, bnComp)) {
        if (nLBTimespan < nFloorTimeLimit) {
            SET_COMPACT(bnNew, INDEX_TARGET(pindexLast));
            bnNew = BIGINT_MULTIPLY(bnNew, arith_uint256(95));
            bnNew = BIGINT_DIVIDE(bnNew, arith_uint256(PERCENT_FACTOR));
        }
        else {
            SET_COMPACT(bnNew, INDEX_TARGET(pindexLast));
        }
    }

    if ((BLOCK_TIME(block) - INDEX_TIME(pindexLast)) > nLongTimeLimit) {
        int64_t nNumMissedSteps = ((BLOCK_TIME(block) - INDEX_TIME(pindexLast) - nLongTimeLimit) / nLongTimeStep) + 1;
        for (int i = 0; i < nNumMissedSteps; ++i) {
            bnNew = BIGINT_MULTIPLY(bnNew, arith_uint256(110));
            bnNew = BIGINT_DIVIDE(bnNew, arith_uint256(PERCENT_FACTOR));
        }
    }

    SET_COMPACT(bnComp, nProofOfWorkLimit);
    if (BIGINT_GREATER_THAN(bnNew, bnComp))
        SET_COMPACT(bnNew, nProofOfWorkLimit);

    return GET_COMPACT(bnNew);
}


unsigned int ZenDifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast)
{
    // ZenCash: https://github.com/zencashio/zen/blob/master/src/pow.cpp
    // Limit adjustment step
    // Use medians to prevent time-warp attacks
    const CBlockIndex* pindexFirst = pindexLast;
    const int64_t nLastBlockTime = pindexLast->GetMedianTimePast();
    const int64_t nFirstBlockTime = pindexFirst->GetMedianTimePast();
    const unsigned int nProofOfWorkLimit = Params().PoWLimit().GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Find the first block in the averaging interval
    arith_uint256 bnTot {0};
    for (int i = 0; pindexFirst && i < Params().PowAveragingWindow(); i++) {
        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindexFirst->nBits);
        bnTot += bnTmp;
        pindexFirst = pindexFirst->pprev;
    }

    // Check we have enough blocks
    if (pindexFirst == NULL)
        return nProofOfWorkLimit;

    arith_uint256 bnAvg {bnTot / Params().PowAveragingWindow()};

    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    LogPrint("pow", "  nActualTimespan = %d  before dampening\n", nActualTimespan);
    nActualTimespan = Params().AveragingWindowTimespan() + (nActualTimespan - Params().AveragingWindowTimespan())/4;
    LogPrint("pow", "  nActualTimespan = %d  before bounds\n", nActualTimespan);

    if (nActualTimespan < Params().MinActualTimespan())
        nActualTimespan = Params().MinActualTimespan();
    if (nActualTimespan > Params().MaxActualTimespan())
        nActualTimespan = Params().MaxActualTimespan();

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(Params().PoWLimit());
    arith_uint256 bnNew {bnAvg};
    bnNew /= Params().AveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrint("pow", "GetNextWorkRequired RETARGET\n");
    LogPrint("pow", "Params().AveragingWindowTimespan() = %d    nActualTimespan = %d\n", Params().AveragingWindowTimespan(), nActualTimespan);
    LogPrint("pow", "Current average: %08x  %s\n", bnAvg.GetCompact(), bnAvg.ToString());
    LogPrint("pow", "After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

unsigned int DGW3DifficultyAdjumentAlgorithm(const CBlockIndex* pindexLast, const CBlockHeader* pblock)
{
    const CBlockIndex* BlockLastSolved = pindexLast;
    const CBlockIndex* BlockReading = pindexLast;
    int64_t nActualTimespan = 0;
    int64_t LastBlockTime = 0;
    int64_t PastBlocksMin = 24; //TODO: CryptoDJ, Increase to 24
    int64_t PastBlocksMax = 24; //TODO: CryptoDJ, Increase to 24
    int64_t CountBlocks = 0;
    uint256 PastDifficultyAverage;
    uint256 PastDifficultyAveragePrev;

    // This is a PoW Block
    // Current difficulty formula, Sling - DarkGravityWave v3, written by Evan Duffield - evan@dashpay.io
    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) {
        return Params().PoWLimit().GetCompact();
    }
    unsigned int nCurrentPoWBlockCount = 1;
    while (true) {
        if (BlockReading->IsProofOfWork()) {
            // Only use Proof-of-Work Blocks
            if (PastBlocksMax > 0 && nCurrentPoWBlockCount > PastBlocksMax) {
                break;
            }
            if (BlockReading->nHeight == 0) {
                break;
            }

            CountBlocks++;

            if (CountBlocks <= PastBlocksMin) {
                if (CountBlocks == 1) {
                    PastDifficultyAverage.SetCompact(BlockReading->nBits);
                } else {
                    PastDifficultyAverage = ((PastDifficultyAveragePrev * CountBlocks) + (uint256().SetCompact(BlockReading->nBits))) / (CountBlocks + 1);
                }
                PastDifficultyAveragePrev = PastDifficultyAverage;
            }

            if (LastBlockTime > 0) {
                int64_t Diff = (LastBlockTime - BlockReading->GetBlockTime());
                nActualTimespan += Diff;
            }
            LastBlockTime = BlockReading->GetBlockTime();

            if (BlockReading->pprev == NULL) {
                assert(BlockReading);
                break;
            }
            //LogPrintf("GetNextWorkRequired: Used PoW nHeight = %d, LastBlockTime = %d, nActualTimespan = %d\n", BlockReading->nHeight, LastBlockTime, nActualTimespan);
            nCurrentPoWBlockCount ++;
        }
        //else
        //    LogPrintf("GetNextWorkRequired: Skipped PoS block number %d\n", BlockReading->nHeight);

        BlockReading = BlockReading->pprev;
    }

    uint256 bnNew(PastDifficultyAverage);

    int64_t _nTargetTimespan = CountBlocks * Params().PoWTargetSpacing();
    //LogPrintf("GetNextWorkRequired: _nTargetTimespan = %d, nActualTimespan = %d\n", _nTargetTimespan, nActualTimespan);

    if (nActualTimespan < _nTargetTimespan / 3)
        nActualTimespan = _nTargetTimespan / 3;
    if (nActualTimespan > _nTargetTimespan * 3)
        nActualTimespan = _nTargetTimespan * 3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= _nTargetTimespan;

    if (bnNew > Params().PoWLimit()) {
        bnNew = Params().PoWLimit();
    }
    //LogPrintf("GetNextWorkRequired: bnNew = %s, Difficulty = %u\n", bnNew.ToString().c_str(), bnNew.GetCompact());
    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits)
{
    bool fNegative;
    bool fOverflow;
    uint256 bnTarget;

    if (Params().SkipProofOfWorkCheck())
        return true;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > Params().PoWLimit())
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount
    if (hash > bnTarget)
        return error("CheckProofOfWork() : hash doesn't match nBits");

    return true;
}

uint256 GetBlockProof(const CBlockIndex& block)
{
    uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}
