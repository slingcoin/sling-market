// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2016-2017 Duality Blockchain Solutions Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HASH_H
#define BITCOIN_HASH_H

#include "crypto/argon2d/argon2.h"
#include "crypto/blake2/blake2.h"
#include "crypto/ripemd160.h"
#include "crypto/sha256.h"

#include "serialize.h"
#include "uint256.h"
#include "version.h"

#include "prevector.h"
#include "serialize.h"
#include "uint256.h"
#include "version.h"


#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <vector>

using namespace std;

static const size_t INPUT_BYTES = 80;  // Lenth of a block header in bytes. Input Length = Salt Length (salt = input)
static const size_t OUTPUT_BYTES = 32; // Length of output needed for a 256-bit hash
static const unsigned int DEFAULT_ARGON2_FLAG = 2; //Same as ARGON2_DEFAULT_FLAGS

/** A hasher class for Bitcoin's 256-bit hash (double SHA-256). */
class CHash256
{
private:
    CSHA256 sha;

public:
    static const size_t OUTPUT_SIZE = CSHA256::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE])
    {
        unsigned char buf[sha.OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, sha.OUTPUT_SIZE).Finalize(hash);
    }

    CHash256& Write(const unsigned char* data, size_t len)
    {
        sha.Write(data, len);
        return *this;
    }

    CHash256& Reset()
    {
        sha.Reset();
        return *this;
    }
};

/* ----------- Bitcoin Hash ------------------------------------------------- */
/** A hasher class for Bitcoin's 160-bit hash (SHA-256 + RIPEMD-160). */
class CHash160
{
private:
    CSHA256 sha;

public:
    static const size_t OUTPUT_SIZE = CRIPEMD160::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE])
    {
        unsigned char buf[sha.OUTPUT_SIZE];
        sha.Finalize(buf);
        CRIPEMD160().Write(buf, sha.OUTPUT_SIZE).Finalize(hash);
    }

    CHash160& Write(const unsigned char* data, size_t len)
    {
        sha.Write(data, len);
        return *this;
    }

    CHash160& Reset()
    {
        sha.Reset();
        return *this;
    }
};

/** Compute the 256-bit hash of a std::string */
inline std::string Hash(std::string input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

/** Compute the 256-bit hash of a void pointer */
inline void Hash(void* in, unsigned int len, unsigned char* out)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, in, len);
    SHA256_Final(out, &sha256);
}

/** Compute the 256-bit hash of an object. */
template <typename T1>
inline uint256 Hash(const T1 pbegin, const T1 pend)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(pbegin == pend ? pblank : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of two objects. */
template <typename T1, typename T2>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of three objects. */
template <typename T1, typename T2, typename T3>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? pblank : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of three objects. */
template <typename T1, typename T2, typename T3, typename T4>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? pblank : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? pblank : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of three objects. */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end, const T5 p5begin, const T5 p5end)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? pblank : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? pblank : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Write(p5begin == p5end ? pblank : (const unsigned char*)&p5begin[0], (p5end - p5begin) * sizeof(p5begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of three objects. */
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end, const T5 p5begin, const T5 p5end, const T6 p6begin, const T6 p6end)
{
    static const unsigned char pblank[1] = {};
    uint256 result;
    CHash256().Write(p1begin == p1end ? pblank : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? pblank : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? pblank : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? pblank : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Write(p5begin == p5end ? pblank : (const unsigned char*)&p5begin[0], (p5end - p5begin) * sizeof(p5begin[0])).Write(p6begin == p6end ? pblank : (const unsigned char*)&p6begin[0], (p6end - p6begin) * sizeof(p6begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash an object. */
template <typename T1>
inline uint160 Hash160(const T1 pbegin, const T1 pend)
{
    static unsigned char pblank[1] = {};
    uint160 result;
    CHash160().Write(pbegin == pend ? pblank : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash of a vector. */
inline uint160 Hash160(const std::vector<unsigned char>& vch)
{
    return Hash160(vch.begin(), vch.end());
}

/** A writer stream (for serialization) that computes a 256-bit hash. */
class CHashWriter
{
private:
    CHash256 ctx;

public:
    int nType;
    int nVersion;

    CHashWriter(int nTypeIn, int nVersionIn) : nType(nTypeIn), nVersion(nVersionIn) {}

    CHashWriter& write(const char* pch, size_t size)
    {
        ctx.Write((const unsigned char*)pch, size);
        return (*this);
    }

    // invalidates the object
    uint256 GetHash()
    {
        uint256 result;
        ctx.Finalize((unsigned char*)&result);
        return result;
    }

    template <typename T>
    CHashWriter& operator<<(const T& obj)
    {
        // Serialize to this stream
        ::Serialize(*this, obj, nType, nVersion);
        return (*this);
    }
};

/** Compute the 256-bit hash of an object's serialization. */
template <typename T>
uint256 SerializeHash(const T& obj, int nType = SER_GETHASH, int nVersion = PROTOCOL_VERSION)
{
    CHashWriter ss(nType, nVersion);
    ss << obj;
    return ss.GetHash();
}

unsigned int MurmurHash3(unsigned int nHashSeed, const std::vector<unsigned char>& vDataToHash);

void BIP32Hash(const unsigned char chainCode[32], unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64]);


/* ----------- Adapted From Dynamic Hash ------------------------------------------------ */
/// https://github.com/duality-solutions/Dynamic
/// Argon2i, Argon2d, and Argon2id are parametrized by:
/// A time cost, which defines the amount of computation realized and therefore the execution time, given in number of iterations
/// A memory cost, which defines the memory usage, given in kibibytes (1 kibibytes = kilobytes 1.024)
/// A parallelism degree, which defines the number of parallel threads

/// Argon2d Phase 1 Hash parameters for the first 9 months - 12 month
/// Salt and password are the block header.
/// Output length: 32 bytes.
/// Input length (in the case of a block header): 80 bytes.
/// Salt length (same note as input length): 80 bytes.
/// Input: Block header
/// Salt: Block header (SAME AS INPUT)
/// Secret data: None
/// Secret length: 0
/// Associated data: None
/// Associated data length: 0
/// Memory cost: 250 kibibytes
/// Lanes: 2 parallel threads
/// Threads: 1 thread
/// Time Constraint: 1 iteration

inline int Argon2d_Phase1_Hash(const void *in, const size_t size, const void *out) {
    argon2_context context;
    context.out = (uint8_t *)out;
    context.outlen = (uint32_t)OUTPUT_BYTES;
    context.pwd = (uint8_t *)in;
    context.pwdlen = (uint32_t)size;
    context.salt = (uint8_t *)in; //salt = input
    context.saltlen = (uint32_t)size;
    context.secret = NULL;
    context.secretlen = 0;
    context.ad = NULL;
    context.adlen = 0;
    context.allocate_cbk = NULL;
    context.free_cbk = NULL;
    context.flags = DEFAULT_ARGON2_FLAG; // = ARGON2_DEFAULT_FLAGS
    // main configurable Argon2 hash parameters
    context.m_cost = 250; // Memory in KiB (~256 KB)
    context.lanes = 2;    // Degree of Parallelism
    context.threads = 1;  // Threads
    context.t_cost = 1;   // Iterations

    return argon2_ctx(&context, Argon2_d);
}

#ifdef __AVX2__

inline int Argon2d_Phase1_Hash_Ctx(const void *in, void *Matrix, void *out) {        
    WolfArgon2dPoWHash(out, Matrix, in);
        
    return(0);
}

#endif

/// Argon2d Phase 2 Hash parameters for the next 5 years after phase 1
/// Salt and password are the block header.
/// Output length: 32 bytes.
/// Input length (in the case of a block header): 80 bytes.
/// Salt length (same note as input length): 80 bytes.
/// Input: Block header
/// Salt: Block header (SAME AS INPUT)
/// Secret data: None
/// Secret length: 0
/// Associated data: None
/// Associated data length: 0
/// Memory cost: 1000 kibibytes
/// Lanes: 32 parallel threads
/// Threads: 1 thread
/// Time Constraint: 8 iterations
inline int Argon2d_Phase2_Hash(const void *in, const size_t size, void *out) {
    argon2_context context;
    context.out = (uint8_t *)out;
    context.outlen = (uint32_t)OUTPUT_BYTES;
    context.pwd = (uint8_t *)in;
    context.pwdlen = (uint32_t)size;
    context.salt = (uint8_t *)in; //salt = input
    context.saltlen = (uint32_t)size;
    context.secret = NULL;
    context.secretlen = 0;
    context.ad = NULL;
    context.adlen = 0;
    context.allocate_cbk = NULL;
    context.free_cbk = NULL;
    context.flags = DEFAULT_ARGON2_FLAG; // = ARGON2_DEFAULT_FLAGS
    // main configurable Argon2 hash parameters
    context.m_cost = 500; // Memory in KiB (~512 KB)
    context.lanes = 32;    // Degree of Parallelism
    context.threads = 1;  // Threads
    context.t_cost = 8;    // Iterations
    
    return argon2_ctx(&context, Argon2_d);
}

inline uint256 hash_Argon2d(const void* input, const unsigned int& hashPhase) {
    uint256 hashResult;
    const uint32_t MaxInt32 = std::numeric_limits<uint32_t>::max();
    if (INPUT_BYTES > MaxInt32 || OUTPUT_BYTES > MaxInt32) {
        return hashResult;
    }
    
    if (hashPhase == 1) {
        Argon2d_Phase1_Hash((const uint8_t*)input, INPUT_BYTES, (uint8_t*)&hashResult);
    }
    else if (hashPhase == 2) {
        Argon2d_Phase2_Hash((const uint8_t*)input, INPUT_BYTES, (uint8_t*)&hashResult);
    }
    else {
        Argon2d_Phase1_Hash((const uint8_t*)input, INPUT_BYTES, (uint8_t*)&hashResult);
    }
    return hashResult;
}

template<typename T1>
inline uint256 hash_Argon2d(const T1 pbegin, const T1 pend, const unsigned int& hashPhase) {
    static unsigned char pblank[1];
    const void* input = (pbegin == pend ? pblank : static_cast<const void*>(&pbegin[0]));
    const size_t size = (pend - pbegin) * sizeof(pbegin[0]);

    uint256 hashResult;
    const uint32_t MaxInt32 = std::numeric_limits<uint32_t>::max();
    if (INPUT_BYTES > MaxInt32 || OUTPUT_BYTES > MaxInt32) {
        return hashResult;
    }
    
    if (hashPhase == 1) {
        Argon2d_Phase1_Hash((const uint8_t*)input, size, (uint8_t*)&hashResult);
    }
    else if (hashPhase == 2) {
        Argon2d_Phase2_Hash((const uint8_t*)input, size, (uint8_t*)&hashResult);
    }
    else {
        Argon2d_Phase1_Hash((const uint8_t*)input, size, (uint8_t*)&hashResult);
    }
    return hashResult;
}

#ifdef __AVX2__

inline uint256 hash_Argon2d_ctx(const void* input, void *Matrix, const unsigned int& hashPhase) {
    uint256 hashResult;
    const uint32_t MaxInt32 = std::numeric_limits<uint32_t>::max();
    if (INPUT_BYTES > MaxInt32 || OUTPUT_BYTES > MaxInt32) {
        return hashResult;
    }
    
    if (hashPhase == 1) {
        Argon2d_Phase1_Hash_Ctx((const uint8_t*)input, Matrix, (uint8_t*)&hashResult);
    }
    else if (hashPhase == 2) {
        Argon2d_Phase2_Hash((const uint8_t*)input, (uint8_t*)&hashResult);
    }
    else {
        Argon2d_Phase1_Hash((const uint8_t*)input, (uint8_t*)&hashResult);
    }
    return hashResult;
}

#endif

#endif // BITCOIN_HASH_H
