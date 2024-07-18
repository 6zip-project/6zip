// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <streams.h>
#include <tinyformat.h>
#include <pow.h>
#include <util/strencodings.h>
#include "logging.h"


std::unordered_set<CBlockHeader, BlockHeaderHash, BlockHeaderEqual> blockSet;


void CBlockHeader::GenerateUniqueID()
{
    uniqueID = Hash(hashPrevBlock.begin(), hashPrevBlock.end(), (unsigned char*)&nTime, (unsigned char*)&nTime + sizeof(nTime));
}

uint256 CBlockHeader::GetUniqueID() const
{
    return uniqueID;
}

void CBlockHeader::LogBlock() const
{
    std::cout << "Block Hash: " << GetHash().ToString() << std::endl;
    std::cout << "Unique ID: " << uniqueID.ToString() << std::endl;
}

std::size_t BlockHeaderHash::operator()(const CBlockHeader& header) const
{
    return std::hash<std::string>()(header.GetUniqueID().ToString());
}

bool BlockHeaderEqual::operator()(const CBlockHeader& lhs, const CBlockHeader& rhs) const
{
    return lhs.GetUniqueID() == rhs.GetUniqueID();
}

bool IsDuplicateBlock(const CBlockHeader& block)
{
    return blockSet.find(block) != blockSet.end();
}

void AddBlockToSet(CBlockHeader& block)
{
    if (!IsDuplicateBlock(block))
    {
        block.GenerateUniqueID();
        blockSet.insert(std::move(block)); // Use std::move here
    }
    else
    {
        std::cerr << "Duplicate block detected: " << block.GetUniqueID().ToString() << std::endl;
    }
}
uint256 CBlockHeader::GetHash() const
{
    std::vector<unsigned char> vch;
    vch.reserve(80); // Reserve the correct size for block header

    // Serialize the block header to the vector
    CVectorWriter ss(SER_GETHASH, PROTOCOL_VERSION, vch, 0);
    ss << nVersion << hashPrevBlock << hashMerkleRoot << nTime << nBits << nNonce;

    // Debug: Log serialized block header data
    LogPrintf("Serialized block header: %s\n", HexStr(MakeUCharSpan(vch)));

    // Use the new HashX7 function with the nonce
    uint256 hash = HashX7((const char *)vch.data(), (const char *)vch.data() + vch.size(), nNonce);

    // Debug: Log hash result
    LogPrintf("Block hash: %s\n", hash.ToString());

    return hash;
}


std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u, uniqueID=%s)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size(),
        uniqueID.ToString()); // Include the unique ID here
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}

static void MarkVersionAsMostRecent(std::list<int32_t>& last_unique_versions, std::list<int32_t>::const_iterator version_it)
{
    if (version_it != last_unique_versions.cbegin()) {
        // Move the found version to the front of the list
        last_unique_versions.splice(last_unique_versions.begin(), last_unique_versions, version_it, std::next(version_it));
    }
}

static void SaveVersionAsMostRecent(std::list<int32_t>& last_unique_versions, const int32_t version)
{
    last_unique_versions.push_front(version);

    // Always keep the last 7 unique versions
    constexpr std::size_t max_backwards_look_ups = 7;
    if (last_unique_versions.size() > max_backwards_look_ups) {
        // Evict the oldest version
        last_unique_versions.pop_back();
    }
}

void CompressibleBlockHeader::Compress(const std::vector<CompressibleBlockHeader>& previous_blocks, std::list<int32_t>& last_unique_versions)
{
    if (previous_blocks.empty()) {
        // Previous block not available, we have to send the block completely uncompressed
        SaveVersionAsMostRecent(last_unique_versions, nVersion);
        return;
    }

    // Try to compress version
    const auto version_it = std::find(last_unique_versions.cbegin(), last_unique_versions.cend(), nVersion);
    if (version_it != last_unique_versions.cend()) {
        // Version is found in the last 7 unique blocks.
        bit_field.SetVersionOffset(static_cast<uint8_t>(std::distance(last_unique_versions.cbegin(), version_it) + 1));

        // Mark the version as the most recent one
        MarkVersionAsMostRecent(last_unique_versions, version_it);
    } else {
        // Save the version as the most recent one
        SaveVersionAsMostRecent(last_unique_versions, nVersion);
    }

    // Previous block is available
    const auto& last_block = previous_blocks.back();
    bit_field.MarkAsCompressed(CompressedHeaderBitField::Flag::PREV_BLOCK_HASH);

    // Compute compressed time diff
    const int64_t time_diff = nTime - last_block.nTime;
    if (time_diff <= std::numeric_limits<int16_t>::max() && time_diff >= std::numeric_limits<int16_t>::min()) {
        time_offset = static_cast<int16_t>(time_diff);
        bit_field.MarkAsCompressed(CompressedHeaderBitField::Flag::TIMESTAMP);
    }

    // If n_bits matches previous block, it can be compressed (not sent at all)
    if (nBits == last_block.nBits) {
        bit_field.MarkAsCompressed(CompressedHeaderBitField::Flag::NBITS);
    }
}

void CompressibleBlockHeader::Uncompress(const std::vector<CBlockHeader>& previous_blocks, std::list<int32_t>& last_unique_versions)
{
    if (previous_blocks.empty()) {
        // First block in chain is always uncompressed
        SaveVersionAsMostRecent(last_unique_versions, nVersion);
        return;
    }

    // We have the previous block
    const auto& last_block = previous_blocks.back();

    // Uncompress version
    if (bit_field.IsVersionCompressed()) {
        const auto version_offset = bit_field.GetVersionOffset();
        if (version_offset <= last_unique_versions.size()) {
            auto version_it = last_unique_versions.begin();
            std::advance(version_it, version_offset - 1);
            nVersion = *version_it;
            MarkVersionAsMostRecent(last_unique_versions, version_it);
        }
    } else {
        // Save the version as the most recent one
        SaveVersionAsMostRecent(last_unique_versions, nVersion);
    }

    // Uncompress prev block hash
    if (bit_field.IsCompressed(CompressedHeaderBitField::Flag::PREV_BLOCK_HASH)) {
        hashPrevBlock = last_block.GetHash();
    }

    // Uncompress timestamp
    if (bit_field.IsCompressed(CompressedHeaderBitField::Flag::TIMESTAMP)) {
        nTime = last_block.nTime + time_offset;
    }

    // Uncompress n_bits
    if (bit_field.IsCompressed(CompressedHeaderBitField::Flag::NBITS)) {
        nBits = last_block.nBits;
    }
}
