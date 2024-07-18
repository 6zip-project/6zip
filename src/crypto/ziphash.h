// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_CUSTOMHASH_H
#define BITCOIN_CRYPTO_CUSTOMHASH_H

#include <crypto/common.h>
#include <span.h>
#include <vector>
#include <cstring>

/** A hasher class for CustomHash (256-bit) */
class CCustomHash256
{
private:
    CSHA256 sha;

public:
    static const size_t OUTPUT_SIZE = 32;

    CCustomHash256() : sha() {}

    CCustomHash256& Write(const unsigned char* data, size_t len) {
        sha.Write(data, len);
        return *this;
    }

    void Finalize(unsigned char hash[OUTPUT_SIZE]) {
        unsigned char buf[OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, OUTPUT_SIZE).Finalize(hash);
    }

    CCustomHash256& Reset() {
        sha.Reset();
        return *this;
    }
};

#endif // BITCOIN_CRYPTO_CUSTOMHASH_H
