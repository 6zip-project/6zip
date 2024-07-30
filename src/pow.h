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
 *
 * The BlockTimeVarianceAdjustment calculates a new difficulty target by averaging the targets of past blocks and adjusting
 * the target based on the actual block time span compared to the expected block time span. Additional
 * behavior is applied based on the current block height and the specified interval.
 *
 * @param pindexLast Pointer to the last block index. This is used to determine the current block's height
 * and retrieve relevant block information for adjustment.
 * @param params Consensus parameters that include various configuration settings for the blockchain,
 * such as the proof-of-work limit and target spacing.
 * @return The new difficulty target in compact format, adjusted according to the block time variance
 * and height-based logic.
 *
 * The adjustment involves:
 * - Calculating the average target over a range of past blocks defined by `nDifficultyAdjustmentRange`
 * - Adjusting the target based on the actual block time span compared to the expected time span
 * - Applying additional logic based on the current block height, with different behavior for every
 *   `nHeightInterval` blocks, such as increasing the maximum allowable target if the height interval is odd.
 *
 * Parameters:
 * - `nDifficultyAdjustmentRange`: Defines the number of blocks used to calculate the average target for
 *   difficulty adjustment. 1000 blocks for mainnet.
 * - `nHeightInterval`: Specifies the interval at which different adjustment logic is applied based on
 *   the block height. Every 10,000 blocks for mainnet.
 */
unsigned int static BlockTimeVarianceAdjustment(const CBlockIndex* pindexLast, const Consensus::Params& params);


/**
 * Compute the custom proof-of-work hash for a block.
 * @param block The block header.
 * @return The computed proof-of-work hash.
 */
//uint256 GetCustomPoWHash(const CBlockHeader& block);

#endif // BITCOIN_POW_H
