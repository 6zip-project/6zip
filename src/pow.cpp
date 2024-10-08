// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <hash.h>
#include <crypto/ziphash.h>
#include <logging.h>



unsigned int static DarkGravityWave(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    /* current difficulty formula, dash - DarkGravity v3, written by Evan Duffield - evan@dash.org */
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    int64_t nPastBlocks = 24;

    // make sure we have at least (nPastBlocks + 1) blocks, otherwise just return powLimit
    if (!pindexLast || pindexLast->nHeight < nPastBlocks) {
        return bnPowLimit.GetCompact();
    }

    const CBlockIndex *pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;

    for (unsigned int nCountBlocks = 1; nCountBlocks <= nPastBlocks; nCountBlocks++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            // NOTE: that's not an average really...
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if(nCountBlocks != nPastBlocks) {
            assert(pindex->pprev); // should never fail
            pindex = pindex->pprev;
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    // NOTE: is this accurate? nActualTimespan counts it for (nPastBlocks - 1) blocks only...
    int64_t nTargetTimespan = nPastBlocks * params.nPowTargetSpacing;

    if (nActualTimespan < nTargetTimespan/3)
        nActualTimespan = nTargetTimespan/3;
    if (nActualTimespan > nTargetTimespan*3)
        nActualTimespan = nTargetTimespan*3;

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}

unsigned int static BlockTimeVarianceAdjustment(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    arith_uint256 bnPowLimit = UintToArith256(params.powLimit); // Remove const to allow modification

    // Make sure we have at least (params.nDifficultyAdjustmentRange + 1) blocks; otherwise, return powLimit
    if (!pindexLast || pindexLast->nHeight < params.nDifficultyAdjustmentRange) {
        return bnPowLimit.GetCompact();
    }

    const CBlockIndex* pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;

    // Calculate average target over past blocks
    for (unsigned int nCountBlocks = 1; nCountBlocks <= params.nDifficultyAdjustmentRange; nCountBlocks++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (nCountBlocks == 1) {
            bnPastTargetAvg = bnTarget;
        } else {
            // Calculate the average target
            bnPastTargetAvg = (bnPastTargetAvg * nCountBlocks + bnTarget) / (nCountBlocks + 1);
        }

        if (nCountBlocks != params.nDifficultyAdjustmentRange) {
            assert(pindex->pprev); // Should never fail
            pindex = pindex->pprev;
        }
    }

    arith_uint256 bnNew(bnPastTargetAvg);
    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    int64_t nTargetTimespan = params.nDifficultyAdjustmentRange * params.nPowTargetSpacing;

    // Adjust the timespan to be within bounds
    if (nActualTimespan < nTargetTimespan / 4) {
        nActualTimespan = nTargetTimespan / 4;
    }
    if (nActualTimespan > nTargetTimespan * 4) {
        nActualTimespan = nTargetTimespan * 4;
    }

    // Retarget
    bnNew *= nActualTimespan;
    bnNew /= nTargetTimespan;

    // Height-based logic for every 10,000 blocks
    int64_t nCurrentHeight = pindexLast->nHeight;
    if ((nCurrentHeight / params.nHeightInterval) % 2 == 1) {
        // Apply different behavior if the height interval is odd
        arith_uint256 bnPowLimitMod = bnPowLimit; // Create a non-const copy for modification
        bnPowLimitMod *= 2; // Increase the maximum allowable target
        bnPowLimit = bnPowLimitMod; // Update the maximum allowable target
    }

    // Ensure the new target does not exceed the proof-of-work limit
    if (bnNew > bnPowLimit) {
        bnNew = bnPowLimit;
    }

    return bnNew.GetCompact();
}


unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
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

    if (pindexLast->nHeight + 1 >= params.nPowRTHeight) {
        return DarkGravityWave(pindexLast, params);
    }

        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Select the appropriate timespan parameter
    int64_t nPowTargetTimespan = (pindexLast->nHeight >= params.nPowRTHeight) ? params.nPowTargetTimespan_v2 : params.nPowTargetTimespan;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < nPowTargetTimespan/4)
        nActualTimespan = nPowTargetTimespan/4;
    if (nActualTimespan > nPowTargetTimespan*4)
        nActualTimespan = nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}


// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

    // Select the appropriate timespan parameter based on the current height
    int64_t nPowTargetTimespan = (height >= params.nPowRTHeight) ? params.nPowTargetTimespan_v2 : params.nPowTargetTimespan;

    int64_t smallest_timespan = nPowTargetTimespan / 4;
    int64_t largest_timespan = nPowTargetTimespan * 4;

    const arith_uint256 pow_limit = UintToArith256(params.powLimit);
    arith_uint256 observed_new_target;
    observed_new_target.SetCompact(new_nbits);

    // Calculate the largest difficulty value possible
    arith_uint256 largest_difficulty_target;
    largest_difficulty_target.SetCompact(old_nbits);
    largest_difficulty_target *= largest_timespan;
    largest_difficulty_target /= nPowTargetTimespan;

    if (largest_difficulty_target > pow_limit) {
        largest_difficulty_target = pow_limit;
    }

    // Round and compare
    arith_uint256 maximum_new_target;
    maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
    if (maximum_new_target < observed_new_target) return false;

    // Calculate the smallest difficulty value possible
    arith_uint256 smallest_difficulty_target;
    smallest_difficulty_target.SetCompact(old_nbits);
    smallest_difficulty_target *= smallest_timespan;
    smallest_difficulty_target /= nPowTargetTimespan;

    if (smallest_difficulty_target > pow_limit) {
        smallest_difficulty_target = pow_limit;
    }

    // Round and compare
    arith_uint256 minimum_new_target;
    minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
    if (minimum_new_target > observed_new_target) return false;

    return true;
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

/*uint256 GetCustomPoWHash(const CBlockHeader& block)
{
    std::vector<unsigned char> customHashOutput(CCustomHash256::OUTPUT_SIZE);
    CCustomHash256().Write((const unsigned char*)&block, sizeof(block)).Finalize(customHashOutput.data());

    uint256 result;
    memcpy(result.begin(), customHashOutput.data(), std::min<size_t>(result.size(), customHashOutput.size()));
    return result;
}
*/
