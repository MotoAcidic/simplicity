// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2019 The Simplicity developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SIMPLICITY_HASH_H
#define SIMPLICITY_HASH_H

#include "crypto/ripemd160.h"
#include "crypto/sha256.h"
#include "init.h"
#include "serialize.h"
#include "uint256.h"
#include "util.h"
#include "version.h"

#include "crypto/sph_blake.h"
#include "crypto/sph_bmw.h"
#include "crypto/sph_groestl.h"
#include "crypto/sph_jh.h"
#include "crypto/sph_keccak.h"
#include "crypto/sph_skein.h"
#include "crypto/sha1.h"
#include "crypto/sha512.h"
#include "crypto/scrypt.h"
#include "crypto/scrypt_opt.h"
#include "crypto/argon2/argon2.h"
#include "crypto/RandomX/src/randomx.h"

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <vector>


typedef uint256 ChainCode;
static const unsigned char PBLANK[1] = {};
static const uint256 ZERO = uint256(0);

/** A hasher class for Bitcoin's 256-bit hash (double SHA-256). */
class CHash256
{
private:
    CSHA256 sha;

public:
    static const size_t OUTPUT_SIZE = CSHA256::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE])
    {
        unsigned char buf[CSHA256::OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, CSHA256::OUTPUT_SIZE).Finalize(hash);
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

class CHash1
{
private:
    CSHA1 sha;

public:
    static const size_t OUTPUT_SIZE = CSHA1::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE])
    {
        unsigned char buf[CSHA1::OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, CSHA1::OUTPUT_SIZE).Finalize(hash);
    }

    CHash1& Write(const unsigned char* data, size_t len)
    {
        sha.Write(data, len);
        return *this;
    }

    CHash1& Reset()
    {
        sha.Reset();
        return *this;
    }
};

class CHash512
{
private:
    CSHA512 sha;

public:
    static const size_t OUTPUT_SIZE = CSHA512::OUTPUT_SIZE;

    void Finalize(unsigned char hash[OUTPUT_SIZE])
    {
        unsigned char buf[CSHA512::OUTPUT_SIZE];
        sha.Finalize(buf);
        sha.Reset().Write(buf, CSHA512::OUTPUT_SIZE).Finalize(hash);
    }

    CHash512& Write(const unsigned char* data, size_t len)
    {
        sha.Write(data, len);
        return *this;
    }

    CHash512& Reset()
    {
        sha.Reset();
        return *this;
    }
};

#ifdef GLOBALDEFINED
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL sph_blake512_context z_blake;
GLOBAL sph_bmw512_context z_bmw;
GLOBAL sph_groestl512_context z_groestl;
GLOBAL sph_jh512_context z_jh;
GLOBAL sph_keccak512_context z_keccak;
GLOBAL sph_skein512_context z_skein;

#define fillz()                          \
    do {                                 \
        sph_blake512_init(&z_blake);     \
        sph_bmw512_init(&z_bmw);         \
        sph_groestl512_init(&z_groestl); \
        sph_jh512_init(&z_jh);           \
        sph_keccak512_init(&z_keccak);   \
        sph_skein512_init(&z_skein);     \
    } while (0)

#define ZBLAKE (memcpy(&ctx_blake, &z_blake, sizeof(z_blake)))
#define ZBMW (memcpy(&ctx_bmw, &z_bmw, sizeof(z_bmw)))
#define ZGROESTL (memcpy(&ctx_groestl, &z_groestl, sizeof(z_groestl)))
#define ZJH (memcpy(&ctx_jh, &z_jh, sizeof(z_jh)))
#define ZKECCAK (memcpy(&ctx_keccak, &z_keccak, sizeof(z_keccak)))
#define ZSKEIN (memcpy(&ctx_skein, &z_skein, sizeof(z_skein)))

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
        unsigned char buf[CSHA256::OUTPUT_SIZE];
        sha.Finalize(buf);
        CRIPEMD160().Write(buf, CSHA256::OUTPUT_SIZE).Finalize(hash);
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
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
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

/** Compute the 512-bit hash of an object. */
template <typename T1>
inline uint512 Hash512(const T1 pbegin, const T1 pend)
{
    uint512 result;
    CHash512().Write(pbegin == pend ? PBLANK : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 512-bit hash of the concatenation of two objects. */
template <typename T1, typename T2>
inline uint512 Hash512(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end)
{
    uint512 result;
    CHash512().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash of an object. */
template <typename T1>
inline uint256 Hash1(const T1 pbegin, const T1 pend)
{
    uint256 result;
    CHash1().Write(pbegin == pend ? PBLANK : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash of the concatenation of two objects. */
template <typename T1, typename T2>
inline uint256 Hash1(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end)
{
    uint256 result;
    CHash1().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of an object. */
template <typename T1>
inline uint256 Hash(const T1 pbegin, const T1 pend)
{
    uint256 result;
    CHash256().Write(pbegin == pend ? PBLANK : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of two objects. */
template <typename T1, typename T2>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end)
{
    uint256 result;
    CHash256().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of three objects. */
template <typename T1, typename T2, typename T3>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end)
{
    uint256 result;
    CHash256().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? PBLANK : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of four objects. */
template <typename T1, typename T2, typename T3, typename T4>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end)
{
    uint256 result;
    CHash256().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? PBLANK : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? PBLANK : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of five objects. */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end, const T5 p5begin, const T5 p5end)
{
    uint256 result;
    CHash256().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? PBLANK : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? PBLANK : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Write(p5begin == p5end ? PBLANK : (const unsigned char*)&p5begin[0], (p5end - p5begin) * sizeof(p5begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 256-bit hash of the concatenation of six objects. */
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline uint256 Hash(const T1 p1begin, const T1 p1end, const T2 p2begin, const T2 p2end, const T3 p3begin, const T3 p3end, const T4 p4begin, const T4 p4end, const T5 p5begin, const T5 p5end, const T6 p6begin, const T6 p6end)
{
    uint256 result;
    CHash256().Write(p1begin == p1end ? PBLANK : (const unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0])).Write(p2begin == p2end ? PBLANK : (const unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0])).Write(p3begin == p3end ? PBLANK : (const unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0])).Write(p4begin == p4end ? PBLANK : (const unsigned char*)&p4begin[0], (p4end - p4begin) * sizeof(p4begin[0])).Write(p5begin == p5end ? PBLANK : (const unsigned char*)&p5begin[0], (p5end - p5begin) * sizeof(p5begin[0])).Write(p6begin == p6end ? PBLANK : (const unsigned char*)&p6begin[0], (p6end - p6begin) * sizeof(p6begin[0])).Finalize((unsigned char*)&result);
    return result;
}

/** Compute the 160-bit hash of an object. */
template <typename T1>
inline uint160 Hash160(const T1 pbegin, const T1 pend)
{
    uint160 result;
    CHash160().Write(pbegin == pend ? PBLANK : (const unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0])).Finalize((unsigned char*)&result);
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

void BIP32Hash(const ChainCode chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64]);

inline uint256 scrypt_hash(const void* input, size_t inputlen, const unsigned int N=1024)
{
    uint256 result;
    scrypt((const char*)input, inputlen, (const char*)input, inputlen, (char*)&result, N, 1, 1, 32);
    return result;
}

inline uint256 scrypt_salted_hash(const void* input, size_t inputlen, const void* salt, size_t saltlen)
{
    uint256 result;
    scrypt((const char*)input, inputlen, (const char*)salt, saltlen, (char*)&result, 1024, 1, 1, 32);
    return result;
}

inline uint256 scrypt_salted_multiround_hash(const void* input, size_t inputlen, const void* salt, size_t saltlen, const unsigned int nRounds)
{
    uint256 resultHash = scrypt_salted_hash(input, inputlen, salt, saltlen);
    uint256 transitionalHash = resultHash;

    for (unsigned int i = 1; i < nRounds; i++) {
        resultHash = scrypt_salted_hash(input, inputlen, (const void*)&transitionalHash, 32);
        transitionalHash = resultHash;
    }

    return resultHash;
}

inline uint256 scrypt_blockhash(const void* input)
{
    //return scrypt_hash(input, 80);
    uint256 result;
    scryptHash(input, (char*)&result, 1024); //fixed length of 80
    return result;
}

inline uint256 scrypt_squared_blockhash(const void* input)
{
    uint256 result;
    scryptHash(input, (char*)&result, 1048576); //fixed length of 80
    return result;
}

//int HMAC_SHA512_Init(HMAC_SHA512_CTX *pctx, const void *pkey, size_t len);
//int HMAC_SHA512_Update(HMAC_SHA512_CTX *pctx, const void *pdata, size_t len);
//int HMAC_SHA512_Final(unsigned char *pmd, HMAC_SHA512_CTX *pctx);

/* ----------- Quark Hash ------------------------------------------------ */
template <typename T1>
inline uint256 HashQuark(const T1 pbegin, const T1 pend)
{
    sph_blake512_context ctx_blake;
    sph_bmw512_context ctx_bmw;
    sph_groestl512_context ctx_groestl;
    sph_jh512_context ctx_jh;
    sph_keccak512_context ctx_keccak;
    sph_skein512_context ctx_skein;

    uint512 mask = 8;
    uint512 zero = 0;

    uint512 hash[9];

    sph_blake512_init(&ctx_blake);
    // ZBLAKE;
    sph_blake512(&ctx_blake, (pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]));
    sph_blake512_close(&ctx_blake, static_cast<void*>(&hash[0]));

    sph_bmw512_init(&ctx_bmw);
    // ZBMW;
    sph_bmw512(&ctx_bmw, static_cast<const void*>(&hash[0]), 64);
    sph_bmw512_close(&ctx_bmw, static_cast<void*>(&hash[1]));

    if ((hash[1] & mask) != zero) {
        sph_groestl512_init(&ctx_groestl);
        // ZGROESTL;
        sph_groestl512(&ctx_groestl, static_cast<const void*>(&hash[1]), 64);
        sph_groestl512_close(&ctx_groestl, static_cast<void*>(&hash[2]));
    } else {
        sph_skein512_init(&ctx_skein);
        // ZSKEIN;
        sph_skein512(&ctx_skein, static_cast<const void*>(&hash[1]), 64);
        sph_skein512_close(&ctx_skein, static_cast<void*>(&hash[2]));
    }

    sph_groestl512_init(&ctx_groestl);
    // ZGROESTL;
    sph_groestl512(&ctx_groestl, static_cast<const void*>(&hash[2]), 64);
    sph_groestl512_close(&ctx_groestl, static_cast<void*>(&hash[3]));

    sph_jh512_init(&ctx_jh);
    // ZJH;
    sph_jh512(&ctx_jh, static_cast<const void*>(&hash[3]), 64);
    sph_jh512_close(&ctx_jh, static_cast<void*>(&hash[4]));

    if ((hash[4] & mask) != zero) {
        sph_blake512_init(&ctx_blake);
        // ZBLAKE;
        sph_blake512(&ctx_blake, static_cast<const void*>(&hash[4]), 64);
        sph_blake512_close(&ctx_blake, static_cast<void*>(&hash[5]));
    } else {
        sph_bmw512_init(&ctx_bmw);
        // ZBMW;
        sph_bmw512(&ctx_bmw, static_cast<const void*>(&hash[4]), 64);
        sph_bmw512_close(&ctx_bmw, static_cast<void*>(&hash[5]));
    }

    sph_keccak512_init(&ctx_keccak);
    // ZKECCAK;
    sph_keccak512(&ctx_keccak, static_cast<const void*>(&hash[5]), 64);
    sph_keccak512_close(&ctx_keccak, static_cast<void*>(&hash[6]));

    sph_skein512_init(&ctx_skein);
    // SKEIN;
    sph_skein512(&ctx_skein, static_cast<const void*>(&hash[6]), 64);
    sph_skein512_close(&ctx_skein, static_cast<void*>(&hash[7]));

    if ((hash[7] & mask) != zero) {
        sph_keccak512_init(&ctx_keccak);
        // ZKECCAK;
        sph_keccak512(&ctx_keccak, static_cast<const void*>(&hash[7]), 64);
        sph_keccak512_close(&ctx_keccak, static_cast<void*>(&hash[8]));
    } else {
        sph_jh512_init(&ctx_jh);
        // ZJH;
        sph_jh512(&ctx_jh, static_cast<const void*>(&hash[7]), 64);
        sph_jh512_close(&ctx_jh, static_cast<void*>(&hash[8]));
    }
    return hash[8].trim256();
}

/* ----------- Scrypt Hash ------------------------------------------------ */
template <typename T1>
inline uint256 HashScrypt(const T1 pbegin, const T1 pend)
{
    uint256 result;
    if ((pend - pbegin) * sizeof(pbegin[0]) != 80 || !scryptHash(static_cast<const void*>(&pbegin[0]), (char*)&result, 1024) || result == ZERO) {
        LogPrintf("Falling back to original implementation to generate normal scrypt hash\n");
        return scrypt_hash((pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]));
    }
    return result;
}

/* ----------- Scrypt² Hash ------------------------------------------------ */
template <typename T1>
inline uint256 HashScryptSquared(const T1 pbegin, const T1 pend)
{
    uint256 result;
    if ((pend - pbegin) * sizeof(pbegin[0]) != 80 || !scryptHash(static_cast<const void*>(&pbegin[0]), (char*)&result, 1048576) || result == ZERO) {
        LogPrintf("Falling back to original implementation to generate scrypt² hash\n");
        return scrypt_hash((pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]), 1048576);
    }
    return result;
}

/* ----------- Argon2d4096 Hash ------------------------------------------------ */
template <typename T1>
inline uint256 HashArgon2d(const T1 pbegin, const T1 pend)
{
    uint32_t t_cost = 1; // 1 iteration
    uint32_t m_cost = 4096; // use 4MB
    uint32_t parallelism = 1; // 1 thread, 2 lanes

    size_t pwdlen = (pend - pbegin) * sizeof(pbegin[0]);

    uint256 result;
    size_t hashlen = 32;

    argon2d_hash_raw(t_cost, m_cost, parallelism, (pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), pwdlen,
                    (pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), pwdlen, (char*)&result, hashlen);

    return result;
}

/* ----------- RandomX Hash ------------------------------------------------ */
template <typename T1>
inline uint256 HashRandomX(const T1 pbegin, const T1 pend)
{
    /*static char seed[4];
    static randomx_flags flags = randomx_get_flags();
    static randomx_cache* cache = randomx_alloc_cache(flags);
    static randomx_dataset* dataset;
    static randomx_vm *vm = nullptr;
    if (vm == nullptr) {
        randomx_init_cache(cache, &seed, sizeof(seed));
        vm = randomx_create_vm(flags, cache, dataset);
    }*/

    uint256 result;
    randomx_calculate_hash(vm, (pbegin == pend ? PBLANK : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]), (char*)&result);
    return result;
}

inline void scrypt_hash(const char* pass, unsigned int pLen, const char* salt, unsigned int sLen, char* output, unsigned int N, unsigned int r, unsigned int p, unsigned int dkLen)
{
    scrypt(pass, pLen, salt, sLen, output, N, r, p, dkLen);
}

#endif // SIMPLICITY_HASH_H