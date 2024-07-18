// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <consensus/params.h>
#include <uint256.h>
#include <vector>

// Forward declarations
class CBlockHeader;
class CBlockIndex;

/**
 * Get the next required proof of work for a new block.
 * @param pindexLast The index of the last block.
 * @param pblock The block header for the new block.
 * @param params Consensus parameters.
 * @return The required proof of work.
 */
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params);

/**
 * Calculate the next required proof of work based on the current difficulty.
 * @param pindexLast The index of the last block.
 * @param nFirstBlockTime The timestamp of the first block in the difficulty interval.
 * @param params Consensus parameters.
 * @return The calculated proof of work.
 */
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params);

/**
 * Check whether the difficulty transition is within permitted limits.
 * @param params Consensus parameters.
 * @param height The height of the block.
 * @param old_nbits The difficulty of the previous block.
 * @param new_nbits The difficulty of the new block.
 * @return True if the transition is permitted, false otherwise.
 */
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits);

/**
 * Check whether a block hash satisfies the proof-of-work requirement.
 * @param hash The block hash.
 * @param nBits The difficulty target.
 * @param params Consensus parameters.
 * @return True if the proof-of-work requirement is satisfied, false otherwise.
 */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params);

/**
 * Compute the custom proof-of-work hash for a block.
 * @param block The block header.
 * @return The computed proof-of-work hash.
 */
//uint256 GetCustomPoWHash(const CBlockHeader& block);

#endif // BITCOIN_POW_H
