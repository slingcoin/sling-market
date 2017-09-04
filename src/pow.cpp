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
    else
    {
        unsigned int nProofOfWorkLimit = Params().PoWLimit().GetCompact();

        // Genesis block
        if (pindexLast == NULL)
            return nProofOfWorkLimit;

        // Find the first block in the averaging interval
        const CBlockIndex* pindexFirst = pindexLast;
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

        return CalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast());
    } 
}

unsigned int CalculateNextWorkRequired(arith_uint256 bnAvg, int64_t nLastBlockTime, int64_t nFirstBlockTime)
{
    // ZenCash: https://github.com/zencashio/zen/blob/master/src/pow.cpp
    // Limit adjustment step
    // Use medians to prevent time-warp attacks
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



/*
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, bool IsProofOfStake)
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

    if (IsProofOfStake) {
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
    else
    {
        // This is a PoW Block
        // Current difficulty formula, Sling - DarkGravityWave v3, written by Evan Duffield - evan@dashpay.io
        if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < PastBlocksMin) {
            return Params().ProofOfWorkLimit().GetCompact();
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

        if (bnNew > Params().ProofOfWorkLimit()) {
            bnNew = Params().ProofOfWorkLimit();
        }
        //LogPrintf("GetNextWorkRequired: bnNew = %s, Difficulty = %u\n", bnNew.ToString().c_str(), bnNew.GetCompact());
        return bnNew.GetCompact();
    }
}
*/

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
