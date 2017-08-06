// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ARITH_UINT256_H
#define BITCOIN_ARITH_UINT256_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

class uint256;

class arith_uint_error : public std::runtime_error
{
public:
    explicit arith_uint_error(const std::string& str) : std::runtime_error(str) {}
};

/** Template base class for unsigned big integers. */
template <unsigned int BITS>
class arith_base_uint
{
protected:
    enum { WIDTH = BITS / 32 };
    uint32_t pn[WIDTH];

public:
    arith_base_uint()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    arith_base_uint(const arith_base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    arith_base_uint& operator=(const arith_base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    arith_base_uint(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    explicit arith_base_uint(const std::string& str);
    explicit arith_base_uint(const std::vector<unsigned char>& vch);

    bool operator!() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (pn[i] != 0)
                return false;
        return true;
    }

    const arith_base_uint operator~() const
    {
        arith_base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        return ret;
    }

    const arith_base_uint operator-() const
    {
        arith_base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        ret++;
        return ret;
    }

    double getdouble() const;

    arith_base_uint& operator=(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    arith_base_uint& operator^=(const arith_base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] ^= b.pn[i];
        return *this;
    }

    arith_base_uint& operator&=(const arith_base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] &= b.pn[i];
        return *this;
    }

    arith_base_uint& operator|=(const arith_base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] |= b.pn[i];
        return *this;
    }

    arith_base_uint& operator^=(uint64_t b)
    {
        pn[0] ^= (unsigned int)b;
        pn[1] ^= (unsigned int)(b >> 32);
        return *this;
    }

    arith_base_uint& operator|=(uint64_t b)
    {
        pn[0] |= (unsigned int)b;
        pn[1] |= (unsigned int)(b >> 32);
        return *this;
    }

    arith_base_uint& operator<<=(unsigned int shift);
    arith_base_uint& operator>>=(unsigned int shift);

    arith_base_uint& operator+=(const arith_base_uint& b)
    {
        uint64_t carry = 0;
        for (int i = 0; i < WIDTH; i++) {
            uint64_t n = carry + pn[i] + b.pn[i];
            pn[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }

    arith_base_uint& operator-=(const arith_base_uint& b)
    {
        *this += -b;
        return *this;
    }

    arith_base_uint& operator+=(uint64_t b64)
    {
        arith_base_uint b;
        b = b64;
        *this += b;
        return *this;
    }

    arith_base_uint& operator-=(uint64_t b64)
    {
        arith_base_uint b;
        b = b64;
        *this += -b;
        return *this;
    }

    arith_base_uint& operator*=(uint32_t b32);
    arith_base_uint& operator*=(const arith_base_uint& b);
    arith_base_uint& operator/=(const arith_base_uint& b);

    arith_base_uint& operator++()
    {
        // prefix operator
        int i = 0;
        while (++pn[i] == 0 && i < WIDTH - 1)
            i++;
        return *this;
    }

    const arith_base_uint operator++(int)
    {
        // postfix operator
        const arith_base_uint ret = *this;
        ++(*this);
        return ret;
    }

    arith_base_uint& operator--()
    {
        // prefix operator
        int i = 0;
        while (--pn[i] == (uint32_t)-1 && i < WIDTH - 1)
            i++;
        return *this;
    }

    const arith_base_uint operator--(int)
    {
        // postfix operator
        const arith_base_uint ret = *this;
        --(*this);
        return ret;
    }

    int CompareTo(const arith_base_uint& b) const;
    bool EqualTo(uint64_t b) const;

    friend inline const arith_base_uint operator+(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) += b; }
    friend inline const arith_base_uint operator-(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) -= b; }
    friend inline const arith_base_uint operator*(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) *= b; }
    friend inline const arith_base_uint operator/(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) /= b; }
    friend inline const arith_base_uint operator|(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) |= b; }
    friend inline const arith_base_uint operator&(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) &= b; }
    friend inline const arith_base_uint operator^(const arith_base_uint& a, const arith_base_uint& b) { return arith_base_uint(a) ^= b; }
    friend inline const arith_base_uint operator>>(const arith_base_uint& a, int shift) { return arith_base_uint(a) >>= shift; }
    friend inline const arith_base_uint operator<<(const arith_base_uint& a, int shift) { return arith_base_uint(a) <<= shift; }
    friend inline const arith_base_uint operator*(const arith_base_uint& a, uint32_t b) { return arith_base_uint(a) *= b; }
    friend inline bool operator==(const arith_base_uint& a, const arith_base_uint& b) { return memcmp(a.pn, b.pn, sizeof(a.pn)) == 0; }
    friend inline bool operator!=(const arith_base_uint& a, const arith_base_uint& b) { return memcmp(a.pn, b.pn, sizeof(a.pn)) != 0; }
    friend inline bool operator>(const arith_base_uint& a, const arith_base_uint& b) { return a.CompareTo(b) > 0; }
    friend inline bool operator<(const arith_base_uint& a, const arith_base_uint& b) { return a.CompareTo(b) < 0; }
    friend inline bool operator>=(const arith_base_uint& a, const arith_base_uint& b) { return a.CompareTo(b) >= 0; }
    friend inline bool operator<=(const arith_base_uint& a, const arith_base_uint& b) { return a.CompareTo(b) <= 0; }
    friend inline bool operator==(const arith_base_uint& a, uint64_t b) { return a.EqualTo(b); }
    friend inline bool operator!=(const arith_base_uint& a, uint64_t b) { return !a.EqualTo(b); }

    std::string GetHex() const;
    void SetHex(const char* psz);
    void SetHex(const std::string& str);
    std::string ToString() const;

    unsigned char* begin()
    {
        return (unsigned char*)&pn[0];
    }

    unsigned char* end()
    {
        return (unsigned char*)&pn[WIDTH];
    }

    const unsigned char* begin() const
    {
        return (unsigned char*)&pn[0];
    }

    const unsigned char* end() const
    {
        return (unsigned char*)&pn[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(pn);
    }

    /**
     * Returns the position of the highest bit set plus one, or zero if the
     * value is zero.
     */
    unsigned int bits() const;

    uint64_t GetLow64() const
    {
        assert(WIDTH >= 2);
        return pn[0] | (uint64_t)pn[1] << 32;
    }
};

/** 256-bit unsigned big integer. */
class arith_uint256 : public arith_base_uint<256>
{
public:
    arith_uint256() {}
    arith_uint256(const arith_base_uint<256>& b) : arith_base_uint<256>(b) {}
    arith_uint256(uint64_t b) : arith_base_uint<256>(b) {}
    explicit arith_uint256(const std::string& str) : arith_base_uint<256>(str) {}
    explicit arith_uint256(const std::vector<unsigned char>& vch) : arith_base_uint<256>(vch) {}

    /**
     * The "compact" format is a representation of a whole
     * number N using an unsigned 32bit number similar to a
     * floating point format.
     * The most significant 8 bits are the unsigned exponent of base 256.
     * This exponent can be thought of as "number of bytes of N".
     * The lower 23 bits are the mantissa.
     * Bit number 24 (0x800000) represents the sign of N.
     * N = (-1^sign) * mantissa * 256^(exponent-3)
     *
     * Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
     * MPI uses the most significant bit of the first byte as sign.
     * Thus 0x1234560000 is compact (0x05123456)
     * and  0xc0de000000 is compact (0x0600c0de)
     *
     * Bitcoin only uses this "compact" format for encoding difficulty
     * targets, which are unsigned 256bit quantities.  Thus, all the
     * complexities of the sign bit and using base 256 are probably an
     * implementation accident.
     */
    arith_uint256& SetCompact(uint32_t nCompact, bool* pfNegative = NULL, bool* pfOverflow = NULL);
    uint32_t GetCompact(bool fNegative = false) const;

    friend uint256 ArithToUint256(const arith_uint256&);
    friend arith_uint256 UintToArith256(const uint256&);
    uint64_t GetHash(const arith_uint256& salt) const;
};

uint256 ArithToUint256(const arith_uint256&);
arith_uint256 UintToArith256(const uint256&);

#endif // BITCOIN_ARITH_UINT256_H
