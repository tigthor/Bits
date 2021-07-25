// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "bignum.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"
#include "version.h"
#include "chainparams.h"

#include <stdexcept>
#include <vector>
#include <openssl/bn.h>
#include <cmath>

unsigned int static KimotoGravityWell(const CBlockIndex* pindexLast, const CBlockHeader *pblock, uint64_t TargetBlocksSpacingSeconds, uint64_t PastBlocksMin, uint64_t PastBlocksMax, const Consensus::Params& params) {
	/* current difficulty formula, megacoin - kimoto gravity well */
	const CBlockIndex  *BlockLastSolved				= pindexLast;
	const CBlockIndex  *BlockReading				= pindexLast;
	const CBlockHeader *BlockCreating				= pblock;
						BlockCreating				= BlockCreating;
	uint64_t			PastBlocksMass				= 0;
	int64_t				PastRateActualSeconds		= 0;
	int64_t				PastRateTargetSeconds		= 0;
	double				PastRateAdjustmentRatio		= double(1);
	CBigNum				PastDifficultyAverage;
	CBigNum				PastDifficultyAveragePrev;
	double				EventHorizonDeviation;
	double				EventHorizonDeviationFast;
	double				EventHorizonDeviationSlow;
	int64_t 			LatestBlockTime 			= BlockLastSolved->GetBlockTime();

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return UintToArith256(params.powLimit).GetCompact(); }

	for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
		if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
		PastBlocksMass++;

		if (i == 1)	{ PastDifficultyAverage.SetCompact(BlockReading->nBits); }
		else		{ PastDifficultyAverage = ((CBigNum().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev; }
		PastDifficultyAveragePrev = PastDifficultyAverage;
		
		if (LatestBlockTime < BlockReading->GetBlockTime()) {
			if (BlockReading->nHeight > 100000) LatestBlockTime = BlockReading->GetBlockTime();
		}

		PastRateActualSeconds			= LatestBlockTime - BlockReading->GetBlockTime();
		if (BlockReading->nHeight > 100000) {
			if (PastRateActualSeconds < 1) { PastRateActualSeconds = 1; }
		} else {
			if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
		}
		
		PastRateTargetSeconds			= TargetBlocksSpacingSeconds * PastBlocksMass;
		PastRateAdjustmentRatio			= double(1);
		//if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
		if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
		PastRateAdjustmentRatio			= double(PastRateTargetSeconds) / double(PastRateActualSeconds);
		}
		EventHorizonDeviation			= 1 + (0.7084 * std::pow((double(PastBlocksMass)/double(28.2)), -1.228));
		EventHorizonDeviationFast		= EventHorizonDeviation;
		EventHorizonDeviationSlow		= 1 / EventHorizonDeviation;

		if (PastBlocksMass >= PastBlocksMin) {
			if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) { assert(BlockReading); break; }
		}
		if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
		BlockReading = BlockReading->pprev;
	}

	CBigNum bnNew(PastDifficultyAverage);
	if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
		bnNew *= PastRateActualSeconds;
		bnNew /= PastRateTargetSeconds;
	}
    if (bnNew > CBigNum(params.powLimit)) { bnNew = CBigNum(params.powLimit); }

	return bnNew.GetCompact();
}


unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params) {
    // current difficulty formula, dash - DarkGravity v3, written by Evan Duffield - evan@dashpay.io 
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    int64_t nActualTimespan = 0;
    int64_t LastBlockTime = 0;
    int64_t PastBlocksMin = 24;
    int64_t PastBlocksMax = 24;
    int64_t CountBlocks = 0;
    CBigNum PastDifficultyAverage;
    CBigNum PastDifficultyAveragePrev;

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || BlockLastSolved->nHeight < params.nSwitchDGW + PastBlocksMin) {
        return UintToArith256(params.powLimit).GetCompact();
    }

    for (unsigned int i = 1; BlockReading && BlockReading->nHeight >= params.nSwitchDGW; i++) {
        if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
        CountBlocks++;

        if(CountBlocks <= PastBlocksMin) {
            if (CountBlocks == 1) { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
            else { PastDifficultyAverage = ((PastDifficultyAveragePrev * CountBlocks)+(CBigNum().SetCompact(BlockReading->nBits))) / (CountBlocks+1); }
            PastDifficultyAveragePrev = PastDifficultyAverage;
        }

        if(LastBlockTime > 0){
            int64_t Diff = (LastBlockTime - BlockReading->GetBlockTime());
            nActualTimespan += Diff;
        }
        LastBlockTime = BlockReading->GetBlockTime();

        if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
        BlockReading = BlockReading->pprev;
    }

    CBigNum bnNew(PastDifficultyAverage);

    int64_t _nTargetTimespan = CountBlocks*params.nPowTargetSpacing;

    if (nActualTimespan < _nTargetTimespan/3)
        nActualTimespan = _nTargetTimespan/3;
    if (nActualTimespan > _nTargetTimespan*3)
        nActualTimespan = _nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= _nTargetTimespan;

    if (bnNew > CBigNum(params.powLimit)){
        bnNew = CBigNum(params.powLimit);
    }

    return bnNew.GetCompact();
}

unsigned int static GetNextWorkRequired_V1(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
	// Traditional Targets only used for KGW v1 blocks
	static const int64_t	BlocksTargetSpacing			= 2.5 * 60;
	static const int64_t	BlocksTargetTimespan		= 3.5 * 24 * 60 * 60;
	int64_t 				nInterval 					= BlocksTargetTimespan / BlocksTargetSpacing;

	unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per interval
    if ((pindexLast->nHeight+1) % nInterval != 0) {
        return pindexLast->nBits;
    }

    // GameCredits: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = nInterval-1;
    if ((pindexLast->nHeight+1) != nInterval)
        blockstogoback = nInterval;

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();

    if (nActualTimespan < BlocksTargetTimespan/4)
        nActualTimespan = BlocksTargetTimespan/4;
    if (nActualTimespan > BlocksTargetTimespan*4)
        nActualTimespan = BlocksTargetTimespan*4;

    // Retarget
    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= BlocksTargetTimespan;

    if (bnNew > CBigNum(params.powLimit))
        bnNew = CBigNum(params.powLimit);

    /// debug print
    //printf("GetNextWorkRequired RETARGET\n");
    //printf("Before: %08x  %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString().c_str());
    //printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

    return bnNew.GetCompact();
}

unsigned int static GetNextWorkRequired_V3(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
	static const int64_t	BlocksTargetSpacing			= params.nPowTargetSpacing;
	static const int64_t	BlocksTargetTimespan		= params.nPowTargetTimespan;
	int64_t 				nIntervalnew 				= BlocksTargetTimespan / BlocksTargetSpacing;

	unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();
	
    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    const CBlockIndex* pindexFirst = pindexLast->pprev;
    int64_t nActualSpacing = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();

    // limit the adjustment
    if (nActualSpacing < BlocksTargetSpacing/16)
      nActualSpacing = BlocksTargetSpacing/16;
    if (nActualSpacing > BlocksTargetSpacing*16)
      nActualSpacing = BlocksTargetSpacing*16;

    // Retarget
    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= ((nIntervalnew - 1) * BlocksTargetSpacing + 2 * nActualSpacing);
    bnNew /= ((nIntervalnew + 1) * BlocksTargetSpacing);

    if (bnNew > CBigNum(params.powLimit))
        bnNew = CBigNum(params.powLimit);

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired_V4(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 90s
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be the last 20 blocks
    // GameCredits: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
        blockstogoback = params.DifficultyAdjustmentInterval();

    // Go back by what we want to be the last 20 blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;

    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
	int64_t					BlocksTargetSpacing			= 0;
	unsigned int			TimeDaySeconds				= 60 * 60 * 24;
	int64_t					PastSecondsMin				= TimeDaySeconds * 0.01;
	int64_t					PastSecondsMax				= TimeDaySeconds * 0.14;
	
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;
if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
}

    if(pindexLast->nHeight+1 < params.nSwitchKGW2)
    {
        // KGW v1
		return GetNextWorkRequired_V1(pindexLast, pblock, params);
    }
    else if(pindexLast->nHeight+1 < params.nSwitchKGW2prefork){
    	// Former 30 Second Block Target, KGW 2 Prefork
		BlocksTargetSpacing = 30;
		uint64_t PastBlocksMin = PastSecondsMin / BlocksTargetSpacing;
		uint64_t PastBlocksMax = PastSecondsMax / BlocksTargetSpacing;
		return KimotoGravityWell(pindexLast, pblock, BlocksTargetSpacing, PastBlocksMin, PastBlocksMax, params);
    }
    else if(pindexLast->nHeight+1 < params.nSwitchKGW2postfork){
    	// New 90 Second Block Target, KGW 2 Postfork
		BlocksTargetSpacing = 90;
		uint64_t PastBlocksMin = PastSecondsMin / BlocksTargetSpacing;
		uint64_t PastBlocksMax = PastSecondsMax / BlocksTargetSpacing;
		return KimotoGravityWell(pindexLast, pblock, BlocksTargetSpacing, PastBlocksMin, PastBlocksMax, params);
    }
	else if(pindexLast->nHeight+1 < params.nSwitchDGW) {
		return GetNextWorkRequired_V3(pindexLast, pblock, params);
	}
	else {
		return DarkGravityWave(pindexLast, pblock, params);
		// If adjusting diff every 20 blocks instead, use below instead of DGW
		// return GetNextWorkRequired_V4(pindexLast, pblock, params);
	}
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    // GameCredits: intermediate uint256 can overflow by 1 bit
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    bool fShift = bnNew.bits() > bnPowLimit.bits() - 1;
    if (fShift)
        bnNew >>= 1;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;
    if (fShift)
        bnNew <<= 1;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
