#include <immintrin.h>				//Intel AVX compile support
#ifndef _ROMANGOL_H_
#define _ROMANGOL_H_

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <inttypes.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

typedef const u1 * const const_buf;
typedef u1 *buf;

class NonCopyable
{
protected:
	NonCopyable () {}
	~NonCopyable () {} /// Protected non-virtual destructor
private: 
	NonCopyable (NonCopyable const &);
	NonCopyable & operator = (NonCopyable const &);
};

#ifndef lenof
#define lenof(x) ( (sizeof((x))) / (sizeof(*(x))))
#endif


/* ------------------------------- rotation ------------------------------- */
	
/* The masking of the right shift is needed to allow n == 0 (using
   just 32 - n and 64 - n results in undefined behaviour). Most uses
   of these macros use a constant and non-zero rotation count. */
#define ROTL32I (x, n) (((x)<<(n)) | ((x)>>((-(n)&31))))
#define ROTL64I (x, n) (((x)<<(n)) | ((x)>>((-(n))&63)))

#define ROTL32(x, n) (SHL((x), (n)) | ((x) >> (32 - (n))))
#define ROTL64(x, n) (u8)( ((x) << (n)) | ( (x) >> (64 - (n))) )

#define ROL32(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROR32(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define ROTL8(x) (((x)<<8)|((x)>>24))
#define ROTL16(x) (((x)<<16)|((x)>>16))
#define ROTL24(x) (((x)<<24)|((x)>>8))

#define SHL32(x,n) (((x) & 0xFFFFFFFF) << (n))

/* ------------------------------- rotation end ------------------------------- */
	
	
/* ------------------------------- packing and unpacking begin ------------------------------- */

#define GET_32BIT_LSB_FIRST(cp) \
  (((u4)(u1)(cp)[0]) | \
  ((u4)(u1)(cp)[1] << 8) | \
  ((u4)(u1)(cp)[2] << 16) | \
  ((u4)(u1)(cp)[3] << 24))

#define PUT_32BIT_LSB_FIRST(cp, value) ( \
  (cp)[0] = (u1)(value), \
  (cp)[1] = (u1)((value) >> 8), \
  (cp)[2] = (u1)((value) >> 16), \
  (cp)[3] = (u1)((value) >> 24) )

#define GET_16BIT_LSB_FIRST(cp) \
  (((u4)(u1)(cp)[0]) | \
  ((u4)(u1)(cp)[1] << 8))

#define PUT_16BIT_LSB_FIRST(cp, value) ( \
  (cp)[0] = (u1)(value), \
  (cp)[1] = (u1)((value) >> 8) )

#define GET_32BIT_MSB_FIRST(cp) \
  (((u4)(u1)(cp)[0] << 24) | \
  ((u4)(u1)(cp)[1] << 16) | \
  ((u4)(u1)(cp)[2] << 8) | \
  ((u4)(u1)(cp)[3]))

#define GET_32BIT(cp) GET_32BIT_MSB_FIRST(cp)

#define PUT_32BIT_MSB_FIRST(cp, value) ( \
  (cp)[0] = (u1)((value) >> 24), \
  (cp)[1] = (u1)((value) >> 16), \
  (cp)[2] = (u1)((value) >> 8), \
  (cp)[3] = (u1)(value) )

#define PUT_32BIT(cp, value) PUT_32BIT_MSB_FIRST(cp, value)

#define GET_16BIT_MSB_FIRST(cp) \
  (((u4)(u1)(cp)[0] << 8) | \
  ((u4)(u1)(cp)[1]))

#define PUT_16BIT_MSB_FIRST(cp, value) ( \
  (cp)[0] = (u1)((value) >> 8), \
  (cp)[1] = (u1)(value) )


#define u128_to_u8(t128, c8) _mm_storeu_si128( (uint128 *)(c8), (t128) );
#define u8_to_u128(c8, t128) (t128) = _mm_loadu_si128( (uint128 *)(c8) );

#ifdef CPU_BIGENDIAN

#define cpu_to_be16(v) (v)
#define cpu_to_be32(v) (v)
#define be16_to_cpu(v) (v)
#define be32_to_cpu(v) (v)

#else

#define cpu_to_le16(v) (v)
#define cpu_to_le32(v) (v)
#define le16_to_cpu(v) (v)
#define le32_to_cpu(v) (v)

#define cpu_to_be16(v) (((v)<< 8) | ((v)>>8))
#define cpu_to_be32(v) (((v)>>24) | (((v)>>8)&0xff00) | (((v)<<8)&0xff0000) | ((v)<<24))
#define be16_to_cpu(v) cpu_to_be16(v)
#define be32_to_cpu(v) cpu_to_be32(v)

#endif


/* Reads a 64-bit integer, in network, big-endian, byte order */
#define READ_UINT64(p)				\
(  (((uint64_t) (p)[0]) << 56)			\
 | (((uint64_t) (p)[1]) << 48)			\
 | (((uint64_t) (p)[2]) << 40)			\
 | (((uint64_t) (p)[3]) << 32)			\
 | (((uint64_t) (p)[4]) << 24)			\
 | (((uint64_t) (p)[5]) << 16)			\
 | (((uint64_t) (p)[6]) << 8)			\
 |  ((uint64_t) (p)[7]))

#define WRITE_UINT64(p, i)			\
do {						\
  (p)[0] = ((i) >> 56) & 0xff;			\
  (p)[1] = ((i) >> 48) & 0xff;			\
  (p)[2] = ((i) >> 40) & 0xff;			\
  (p)[3] = ((i) >> 32) & 0xff;			\
  (p)[4] = ((i) >> 24) & 0xff;			\
  (p)[5] = ((i) >> 16) & 0xff;			\
  (p)[6] = ((i) >> 8) & 0xff;			\
  (p)[7] = (i) & 0xff;				\
} while(0)

/* Reads a 32-bit integer, in network, big-endian, byte order */
#define READ_UINT32(p)				\
(  (((uint32_t) (p)[0]) << 24)			\
 | (((uint32_t) (p)[1]) << 16)			\
 | (((uint32_t) (p)[2]) << 8)			\
 |  ((uint32_t) (p)[3]))

#define WRITE_UINT32(p, i)			\
do {						\
  (p)[0] = ((i) >> 24) & 0xff;			\
  (p)[1] = ((i) >> 16) & 0xff;			\
  (p)[2] = ((i) >> 8) & 0xff;			\
  (p)[3] = (i) & 0xff;				\
} while(0)

/* Analogous macros, for 24 and 16 bit numbers */
#define READ_UINT24(p)				\
(  (((uint32_t) (p)[0]) << 16)			\
 | (((uint32_t) (p)[1]) << 8)			\
 |  ((uint32_t) (p)[2]))

#define WRITE_UINT24(p, i)			\
do {						\
  (p)[0] = ((i) >> 16) & 0xff;			\
  (p)[1] = ((i) >> 8) & 0xff;			\
  (p)[2] = (i) & 0xff;				\
} while(0)

#define READ_UINT16(p)				\
(  (((uint32_t) (p)[0]) << 8)			\
 |  ((uint32_t) (p)[1]))

#define WRITE_UINT16(p, i)			\
do {						\
  (p)[0] = ((i) >> 8) & 0xff;			\
  (p)[1] = (i) & 0xff;				\
} while(0)

/* And the other, little-endian, byteorder */
#define LE_READ_UINT64(p)			\
(  (((uint64_t) (p)[7]) << 56)			\
 | (((uint64_t) (p)[6]) << 48)			\
 | (((uint64_t) (p)[5]) << 40)			\
 | (((uint64_t) (p)[4]) << 32)			\
 | (((uint64_t) (p)[3]) << 24)			\
 | (((uint64_t) (p)[2]) << 16)			\
 | (((uint64_t) (p)[1]) << 8)			\
 |  ((uint64_t) (p)[0]))

#define LE_WRITE_UINT64(p, i)			\
do {						\
  (p)[7] = ((i) >> 56) & 0xff;			\
  (p)[6] = ((i) >> 48) & 0xff;			\
  (p)[5] = ((i) >> 40) & 0xff;			\
  (p)[4] = ((i) >> 32) & 0xff;			\
  (p)[3] = ((i) >> 24) & 0xff;			\
  (p)[2] = ((i) >> 16) & 0xff;			\
  (p)[1] = ((i) >> 8) & 0xff;			\
  (p)[0] = (i) & 0xff;				\
} while (0)

#define LE_READ_UINT32(p)			\
(  (((uint32_t) (p)[3]) << 24)			\
 | (((uint32_t) (p)[2]) << 16)			\
 | (((uint32_t) (p)[1]) << 8)			\
 |  ((uint32_t) (p)[0]))

#define LE_WRITE_UINT32(p, i)			\
do {						\
  (p)[3] = ((i) >> 24) & 0xff;			\
  (p)[2] = ((i) >> 16) & 0xff;			\
  (p)[1] = ((i) >> 8) & 0xff;			\
  (p)[0] = (i) & 0xff;				\
} while(0)

/* Analogous macros, for 16 bit numbers */
#define LE_READ_UINT16(p)			\
  (  (((uint32_t) (p)[1]) << 8)			\
     |  ((uint32_t) (p)[0]))

#define LE_WRITE_UINT16(p, i)			\
  do {						\
    (p)[1] = ((i) >> 8) & 0xff;			\
    (p)[0] = (i) & 0xff;			\
  } while(0)


/* from libsodium */
#define LOAD64_LE(SRC) load64_le(SRC)
static inline uint64_t
load64_le(const uint8_t src[8])
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint64_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint64_t w = (uint64_t) src[0];
    w |= (uint64_t) src[1] <<  8;
    w |= (uint64_t) src[2] << 16;
    w |= (uint64_t) src[3] << 24;
    w |= (uint64_t) src[4] << 32;
    w |= (uint64_t) src[5] << 40;
    w |= (uint64_t) src[6] << 48;
    w |= (uint64_t) src[7] << 56;
    return w;
#endif
}

#define STORE64_LE(DST, W) store64_le((DST), (W))
static inline void
store64_le(uint8_t dst[8], uint64_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[0] = (uint8_t) w; w >>= 8;
    dst[1] = (uint8_t) w; w >>= 8;
    dst[2] = (uint8_t) w; w >>= 8;
    dst[3] = (uint8_t) w; w >>= 8;
    dst[4] = (uint8_t) w; w >>= 8;
    dst[5] = (uint8_t) w; w >>= 8;
    dst[6] = (uint8_t) w; w >>= 8;
    dst[7] = (uint8_t) w;
#endif
}

#define LOAD32_LE(SRC) load32_le(SRC)
static inline uint32_t
load32_le(const uint8_t src[4])
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint32_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint32_t w = (uint32_t) src[0];
    w |= (uint32_t) src[1] <<  8;
    w |= (uint32_t) src[2] << 16;
    w |= (uint32_t) src[3] << 24;
    return w;
#endif
}

#define STORE32_LE(DST, W) store32_le((DST), (W))
static inline void
store32_le(uint8_t dst[4], uint32_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[0] = (uint8_t) w; w >>= 8;
    dst[1] = (uint8_t) w; w >>= 8;
    dst[2] = (uint8_t) w; w >>= 8;
    dst[3] = (uint8_t) w;
#endif
}

/* ----- */

#define LOAD64_BE(SRC) load64_be(SRC)
static inline uint64_t
load64_be(const uint8_t src[8])
{
#ifdef NATIVE_BIG_ENDIAN
    uint64_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint64_t w = (uint64_t) src[7];
    w |= (uint64_t) src[6] <<  8;
    w |= (uint64_t) src[5] << 16;
    w |= (uint64_t) src[4] << 24;
    w |= (uint64_t) src[3] << 32;
    w |= (uint64_t) src[2] << 40;
    w |= (uint64_t) src[1] << 48;
    w |= (uint64_t) src[0] << 56;
    return w;
#endif
}

#define LOAD32_BE(SRC) load32_be(SRC)
static inline uint32_t
load32_be(const uint8_t src[4])
{
#ifdef NATIVE_BIG_ENDIAN
    uint32_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint32_t w = (uint32_t) src[3];
    w |= (uint32_t) src[2] <<  8;
    w |= (uint32_t) src[1] << 16;
    w |= (uint32_t) src[0] << 24;
    return w;
#endif
}

#define STORE64_BE(DST, W) store64_be((DST), (W))
static inline void
store64_be(uint8_t dst[8], uint64_t w)
{
#ifdef NATIVE_BIG_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[7] = (uint8_t) w; w >>= 8;
    dst[6] = (uint8_t) w; w >>= 8;
    dst[5] = (uint8_t) w; w >>= 8;
    dst[4] = (uint8_t) w; w >>= 8;
    dst[3] = (uint8_t) w; w >>= 8;
    dst[2] = (uint8_t) w; w >>= 8;
    dst[1] = (uint8_t) w; w >>= 8;
    dst[0] = (uint8_t) w;
#endif
}

#define STORE32_BE(DST, W) store32_be((DST), (W))
static inline void
store32_be(uint8_t dst[4], uint32_t w)
{
#ifdef NATIVE_BIG_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[3] = (uint8_t) w; w >>= 8;
    dst[2] = (uint8_t) w; w >>= 8;
    dst[1] = (uint8_t) w; w >>= 8;
    dst[0] = (uint8_t) w;
#endif
}
/* ------------------------------- packing and unpacking end ------------------------------- */
	
	
	
	
/* ------------------------------- loop begin ------------------------------- */

#define forloop(i, start, end) for ( size_t (i) = (start); (i) < (end); ++(i) )
#define forstep(i, start, end, step) for ( size_t (i) = (start); (i) < (end); (i) += (step) )


/* Macro to make it easier to loop over several blocks. */
#define FOR_BLOCKS(length, dst, src, blocksize)	\
  assert( !((length) % (blocksize)));           \
  for (; (length); ((length) -= (blocksize),	\
		  (dst) += (blocksize),		\
		  (src) += (blocksize)) )

/* ------------------------------- loop end ------------------------------- */
	
/* ------------------------------- arithmatic begin ------------------------------- */
#define bitswap(L, R, n, mask) ( swap = mask & ( (R >> n) ^ L ), R ^= swap << n, L ^= swap)
/* ------------------------------- arithmatic end ------------------------------- */

#endif // end of romangol.h

const static size_t SM4_BLOCK_SIZE = 16;
const static size_t SM4_KEY_SIZE = 16;
const static size_t SM4_RND_KEY_SIZE = 32 * 4;
void SM4_key_schedule(const u1 key[SM4_KEY_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)]);
void sms4_avx2_encrypt_init(const u4 *key);
void sms4_avx2_encrypt_16blocks(const unsigned char *in, int *out, const u4 *key);
void sms4_avx2_decrypt_16blocks(const unsigned char *in, int *out, const u4 *key);
void sms4_avx2_encrypt_blocks(const unsigned char *in, int *out, const u4 *key, u4 BLK_CNT);
void sms4_avx2_decrypt_blocks(const unsigned char *in, int *out, const u4 *key, u4 BLK_CNT);
void benchmark_sm4_encrypt(const uint8_t *p, u1 *c, const u4 *key, unsigned int n_block);
void benchmark_sm4_decrypt(const uint8_t *p, u1 *c, const u4 *key, unsigned int n_block);
const u1 Sbox[256] =
{
	0xD6, 0x90, 0xE9, 0xFE, 0xCC, 0xE1, 0x3D, 0xB7, 0x16, 0xB6, 0x14, 0xC2, 0x28, 0xFB, 0x2C, 0x05,
	0x2B, 0x67, 0x9A, 0x76, 0x2A, 0xBE, 0x04, 0xC3, 0xAA, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
	0x9C, 0x42, 0x50, 0xF4, 0x91, 0xEF, 0x98, 0x7A, 0x33, 0x54, 0x0B, 0x43, 0xED, 0xCF, 0xAC, 0x62,
	0xE4, 0xB3, 0x1C, 0xA9, 0xC9, 0x08, 0xE8, 0x95, 0x80, 0xDF, 0x94, 0xFA, 0x75, 0x8F, 0x3F, 0xA6,
	0x47, 0x07, 0xA7, 0xFC, 0xF3, 0x73, 0x17, 0xBA, 0x83, 0x59, 0x3C, 0x19, 0xE6, 0x85, 0x4F, 0xA8,
	0x68, 0x6B, 0x81, 0xB2, 0x71, 0x64, 0xDA, 0x8B, 0xF8, 0xEB, 0x0F, 0x4B, 0x70, 0x56, 0x9D, 0x35,
	0x1E, 0x24, 0x0E, 0x5E, 0x63, 0x58, 0xD1, 0xA2, 0x25, 0x22, 0x7C, 0x3B, 0x01, 0x21, 0x78, 0x87,
	0xD4, 0x00, 0x46, 0x57, 0x9F, 0xD3, 0x27, 0x52, 0x4C, 0x36, 0x02, 0xE7, 0xA0, 0xC4, 0xC8, 0x9E,
	0xEA, 0xBF, 0x8A, 0xD2, 0x40, 0xC7, 0x38, 0xB5, 0xA3, 0xF7, 0xF2, 0xCE, 0xF9, 0x61, 0x15, 0xA1,
	0xE0, 0xAE, 0x5D, 0xA4, 0x9B, 0x34, 0x1A, 0x55, 0xAD, 0x93, 0x32, 0x30, 0xF5, 0x8C, 0xB1, 0xE3,
	0x1D, 0xF6, 0xE2, 0x2E, 0x82, 0x66, 0xCA, 0x60, 0xC0, 0x29, 0x23, 0xAB, 0x0D, 0x53, 0x4E, 0x6F,
	0xD5, 0xDB, 0x37, 0x45, 0xDE, 0xFD, 0x8E, 0x2F, 0x03, 0xFF, 0x6A, 0x72, 0x6D, 0x6C, 0x5B, 0x51,
	0x8D, 0x1B, 0xAF, 0x92, 0xBB, 0xDD, 0xBC, 0x7F, 0x11, 0xD9, 0x5C, 0x41, 0x1F, 0x10, 0x5A, 0xD8,
	0x0A, 0xC1, 0x31, 0x88, 0xA5, 0xCD, 0x7B, 0xBD, 0x2D, 0x74, 0xD0, 0x12, 0xB8, 0xE5, 0xB4, 0xB0,
	0x89, 0x69, 0x97, 0x4A, 0x0C, 0x96, 0x77, 0x7E, 0x65, 0xB9, 0xF1, 0x09, 0xC5, 0x6E, 0xC6, 0x84,
	0x18, 0xF0, 0x7D, 0xEC, 0x3A, 0xDC, 0x4D, 0x20, 0x79, 0xEE, 0x5F, 0x3E, 0xD7, 0xCB, 0x39, 0x48
};

/*
* Sbox_T[j] == L(Sbox[j]).
*/
const u4 Sbox_T[256] = {
0x8ED55B5B, 0xD0924242, 0x4DEAA7A7, 0x06FDFBFB, 0xFCCF3333, 0x65E28787,
0xC93DF4F4, 0x6BB5DEDE, 0x4E165858, 0x6EB4DADA, 0x44145050, 0xCAC10B0B,
0x8828A0A0, 0x17F8EFEF, 0x9C2CB0B0, 0x11051414, 0x872BACAC, 0xFB669D9D,
0xF2986A6A, 0xAE77D9D9, 0x822AA8A8, 0x46BCFAFA, 0x14041010, 0xCFC00F0F,
0x02A8AAAA, 0x54451111, 0x5F134C4C, 0xBE269898, 0x6D482525, 0x9E841A1A,
0x1E061818, 0xFD9B6666, 0xEC9E7272, 0x4A430909, 0x10514141, 0x24F7D3D3,
0xD5934646, 0x53ECBFBF, 0xF89A6262, 0x927BE9E9, 0xFF33CCCC, 0x04555151,
0x270B2C2C, 0x4F420D0D, 0x59EEB7B7, 0xF3CC3F3F, 0x1CAEB2B2, 0xEA638989,
0x74E79393, 0x7FB1CECE, 0x6C1C7070, 0x0DABA6A6, 0xEDCA2727, 0x28082020,
0x48EBA3A3, 0xC1975656, 0x80820202, 0xA3DC7F7F, 0xC4965252, 0x12F9EBEB,
0xA174D5D5, 0xB38D3E3E, 0xC33FFCFC, 0x3EA49A9A, 0x5B461D1D, 0x1B071C1C,
0x3BA59E9E, 0x0CFFF3F3, 0x3FF0CFCF, 0xBF72CDCD, 0x4B175C5C, 0x52B8EAEA,
0x8F810E0E, 0x3D586565, 0xCC3CF0F0, 0x7D196464, 0x7EE59B9B, 0x91871616,
0x734E3D3D, 0x08AAA2A2, 0xC869A1A1, 0xC76AADAD, 0x85830606, 0x7AB0CACA,
0xB570C5C5, 0xF4659191, 0xB2D96B6B, 0xA7892E2E, 0x18FBE3E3, 0x47E8AFAF,
0x330F3C3C, 0x674A2D2D, 0xB071C1C1, 0x0E575959, 0xE99F7676, 0xE135D4D4,
0x661E7878, 0xB4249090, 0x360E3838, 0x265F7979, 0xEF628D8D, 0x38596161,
0x95D24747, 0x2AA08A8A, 0xB1259494, 0xAA228888, 0x8C7DF1F1, 0xD73BECEC,
0x05010404, 0xA5218484, 0x9879E1E1, 0x9B851E1E, 0x84D75353, 0x00000000,
0x5E471919, 0x0B565D5D, 0xE39D7E7E, 0x9FD04F4F, 0xBB279C9C, 0x1A534949,
0x7C4D3131, 0xEE36D8D8, 0x0A020808, 0x7BE49F9F, 0x20A28282, 0xD4C71313,
0xE8CB2323, 0xE69C7A7A, 0x42E9ABAB, 0x43BDFEFE, 0xA2882A2A, 0x9AD14B4B,
0x40410101, 0xDBC41F1F, 0xD838E0E0, 0x61B7D6D6, 0x2FA18E8E, 0x2BF4DFDF,
0x3AF1CBCB, 0xF6CD3B3B, 0x1DFAE7E7, 0xE5608585, 0x41155454, 0x25A38686,
0x60E38383, 0x16ACBABA, 0x295C7575, 0x34A69292, 0xF7996E6E, 0xE434D0D0,
0x721A6868, 0x01545555, 0x19AFB6B6, 0xDF914E4E, 0xFA32C8C8, 0xF030C0C0,
0x21F6D7D7, 0xBC8E3232, 0x75B3C6C6, 0x6FE08F8F, 0x691D7474, 0x2EF5DBDB,
0x6AE18B8B, 0x962EB8B8, 0x8A800A0A, 0xFE679999, 0xE2C92B2B, 0xE0618181,
0xC0C30303, 0x8D29A4A4, 0xAF238C8C, 0x07A9AEAE, 0x390D3434, 0x1F524D4D,
0x764F3939, 0xD36EBDBD, 0x81D65757, 0xB7D86F6F, 0xEB37DCDC, 0x51441515,
0xA6DD7B7B, 0x09FEF7F7, 0xB68C3A3A, 0x932FBCBC, 0x0F030C0C, 0x03FCFFFF,
0xC26BA9A9, 0xBA73C9C9, 0xD96CB5B5, 0xDC6DB1B1, 0x375A6D6D, 0x15504545,
0xB98F3636, 0x771B6C6C, 0x13ADBEBE, 0xDA904A4A, 0x57B9EEEE, 0xA9DE7777,
0x4CBEF2F2, 0x837EFDFD, 0x55114444, 0xBDDA6767, 0x2C5D7171, 0x45400505,
0x631F7C7C, 0x50104040, 0x325B6969, 0xB8DB6363, 0x220A2828, 0xC5C20707,
0xF531C4C4, 0xA88A2222, 0x31A79696, 0xF9CE3737, 0x977AEDED, 0x49BFF6F6,
0x992DB4B4, 0xA475D1D1, 0x90D34343, 0x5A124848, 0x58BAE2E2, 0x71E69797,
0x64B6D2D2, 0x70B2C2C2, 0xAD8B2626, 0xCD68A5A5, 0xCB955E5E, 0x624B2929,
0x3C0C3030, 0xCE945A5A, 0xAB76DDDD, 0x867FF9F9, 0xF1649595, 0x5DBBE6E6,
0x35F2C7C7, 0x2D092424, 0xD1C61717, 0xD66FB9B9, 0xDEC51B1B, 0x94861212,
0x78186060, 0x30F3C3C3, 0x897CF5F5, 0x5CEFB3B3, 0xD23AE8E8, 0xACDF7373,
0x794C3535, 0xA0208080, 0x9D78E5E5, 0x56EDBBBB, 0x235E7D7D, 0xC63EF8F8,
0x8BD45F5F, 0xE7C82F2F, 0xDD39E4E4, 0x68492121 };

static u4 Sbox_T8[256] =
{
0x5b8ed55b, 0x42d09242, 0xa74deaa7, 0xfb06fdfb,
0x33fccf33, 0x8765e287, 0xf4c93df4, 0xde6bb5de,
0x584e1658, 0xda6eb4da, 0x50441450, 0x0bcac10b,
0xa08828a0, 0xef17f8ef, 0xb09c2cb0, 0x14110514,
0xac872bac, 0x9dfb669d, 0x6af2986a, 0xd9ae77d9,
0xa8822aa8, 0xfa46bcfa, 0x10140410, 0x0fcfc00f,
0xaa02a8aa, 0x11544511, 0x4c5f134c, 0x98be2698,
0x256d4825, 0x1a9e841a, 0x181e0618, 0x66fd9b66,
0x72ec9e72, 0x094a4309, 0x41105141, 0xd324f7d3,
0x46d59346, 0xbf53ecbf, 0x62f89a62, 0xe9927be9,
0xccff33cc, 0x51045551, 0x2c270b2c, 0x0d4f420d,
0xb759eeb7, 0x3ff3cc3f, 0xb21caeb2, 0x89ea6389,
0x9374e793, 0xce7fb1ce, 0x706c1c70, 0xa60daba6,
0x27edca27, 0x20280820, 0xa348eba3, 0x56c19756,
0x02808202, 0x7fa3dc7f, 0x52c49652, 0xeb12f9eb,
0xd5a174d5, 0x3eb38d3e, 0xfcc33ffc, 0x9a3ea49a,
0x1d5b461d, 0x1c1b071c, 0x9e3ba59e, 0xf30cfff3,
0xcf3ff0cf, 0xcdbf72cd, 0x5c4b175c, 0xea52b8ea,
0x0e8f810e, 0x653d5865, 0xf0cc3cf0, 0x647d1964,
0x9b7ee59b, 0x16918716, 0x3d734e3d, 0xa208aaa2,
0xa1c869a1, 0xadc76aad, 0x06858306, 0xca7ab0ca,
0xc5b570c5, 0x91f46591, 0x6bb2d96b, 0x2ea7892e,
0xe318fbe3, 0xaf47e8af, 0x3c330f3c, 0x2d674a2d,
0xc1b071c1, 0x590e5759, 0x76e99f76, 0xd4e135d4,
0x78661e78, 0x90b42490, 0x38360e38, 0x79265f79,
0x8def628d, 0x61385961, 0x4795d247, 0x8a2aa08a,
0x94b12594, 0x88aa2288, 0xf18c7df1, 0xecd73bec,
0x04050104, 0x84a52184, 0xe19879e1, 0x1e9b851e,
0x5384d753, 0x00000000, 0x195e4719, 0x5d0b565d,
0x7ee39d7e, 0x4f9fd04f, 0x9cbb279c, 0x491a5349,
0x317c4d31, 0xd8ee36d8, 0x080a0208, 0x9f7be49f,
0x8220a282, 0x13d4c713, 0x23e8cb23, 0x7ae69c7a,
0xab42e9ab, 0xfe43bdfe, 0x2aa2882a, 0x4b9ad14b,
0x01404101, 0x1fdbc41f, 0xe0d838e0, 0xd661b7d6,
0x8e2fa18e, 0xdf2bf4df, 0xcb3af1cb, 0x3bf6cd3b,
0xe71dfae7, 0x85e56085, 0x54411554, 0x8625a386,
0x8360e383, 0xba16acba, 0x75295c75, 0x9234a692,
0x6ef7996e, 0xd0e434d0, 0x68721a68, 0x55015455,
0xb619afb6, 0x4edf914e, 0xc8fa32c8, 0xc0f030c0,
0xd721f6d7, 0x32bc8e32, 0xc675b3c6, 0x8f6fe08f,
0x74691d74, 0xdb2ef5db, 0x8b6ae18b, 0xb8962eb8,
0x0a8a800a, 0x99fe6799, 0x2be2c92b, 0x81e06181,
0x03c0c303, 0xa48d29a4, 0x8caf238c, 0xae07a9ae,
0x34390d34, 0x4d1f524d, 0x39764f39, 0xbdd36ebd,
0x5781d657, 0x6fb7d86f, 0xdceb37dc, 0x15514415,
0x7ba6dd7b, 0xf709fef7, 0x3ab68c3a, 0xbc932fbc,
0x0c0f030c, 0xff03fcff, 0xa9c26ba9, 0xc9ba73c9,
0xb5d96cb5, 0xb1dc6db1, 0x6d375a6d, 0x45155045,
0x36b98f36, 0x6c771b6c, 0xbe13adbe, 0x4ada904a,
0xee57b9ee, 0x77a9de77, 0xf24cbef2, 0xfd837efd,
0x44551144, 0x67bdda67, 0x712c5d71, 0x05454005,
0x7c631f7c, 0x40501040, 0x69325b69, 0x63b8db63,
0x28220a28, 0x07c5c207, 0xc4f531c4, 0x22a88a22,
0x9631a796, 0x37f9ce37, 0xed977aed, 0xf649bff6,
0xb4992db4, 0xd1a475d1, 0x4390d343, 0x485a1248,
0xe258bae2, 0x9771e697, 0xd264b6d2, 0xc270b2c2,
0x26ad8b26, 0xa5cd68a5, 0x5ecb955e, 0x29624b29,
0x303c0c30, 0x5ace945a, 0xddab76dd, 0xf9867ff9,
0x95f16495, 0xe65dbbe6, 0xc735f2c7, 0x242d0924,
0x17d1c617, 0xb9d66fb9, 0x1bdec51b, 0x12948612,
0x60781860, 0xc330f3c3, 0xf5897cf5, 0xb35cefb3,
0xe8d23ae8, 0x73acdf73, 0x35794c35, 0x80a02080,
0xe59d78e5, 0xbb56edbb, 0x7d235e7d, 0xf8c63ef8,
0x5f8bd45f, 0x2fe7c82f, 0xe4dd39e4, 0x21684921,
};

static u4 Sbox_T16[256] =
{
0x5b5b8ed5, 0x4242d092, 0xa7a74dea, 0xfbfb06fd,
0x3333fccf, 0x878765e2, 0xf4f4c93d, 0xdede6bb5,
0x58584e16, 0xdada6eb4, 0x50504414, 0x0b0bcac1,
0xa0a08828, 0xefef17f8, 0xb0b09c2c, 0x14141105,
0xacac872b, 0x9d9dfb66, 0x6a6af298, 0xd9d9ae77,
0xa8a8822a, 0xfafa46bc, 0x10101404, 0x0f0fcfc0,
0xaaaa02a8, 0x11115445, 0x4c4c5f13, 0x9898be26,
0x25256d48, 0x1a1a9e84, 0x18181e06, 0x6666fd9b,
0x7272ec9e, 0x09094a43, 0x41411051, 0xd3d324f7,
0x4646d593, 0xbfbf53ec, 0x6262f89a, 0xe9e9927b,
0xccccff33, 0x51510455, 0x2c2c270b, 0x0d0d4f42,
0xb7b759ee, 0x3f3ff3cc, 0xb2b21cae, 0x8989ea63,
0x939374e7, 0xcece7fb1, 0x70706c1c, 0xa6a60dab,
0x2727edca, 0x20202808, 0xa3a348eb, 0x5656c197,
0x02028082, 0x7f7fa3dc, 0x5252c496, 0xebeb12f9,
0xd5d5a174, 0x3e3eb38d, 0xfcfcc33f, 0x9a9a3ea4,
0x1d1d5b46, 0x1c1c1b07, 0x9e9e3ba5, 0xf3f30cff,
0xcfcf3ff0, 0xcdcdbf72, 0x5c5c4b17, 0xeaea52b8,
0x0e0e8f81, 0x65653d58, 0xf0f0cc3c, 0x64647d19,
0x9b9b7ee5, 0x16169187, 0x3d3d734e, 0xa2a208aa,
0xa1a1c869, 0xadadc76a, 0x06068583, 0xcaca7ab0,
0xc5c5b570, 0x9191f465, 0x6b6bb2d9, 0x2e2ea789,
0xe3e318fb, 0xafaf47e8, 0x3c3c330f, 0x2d2d674a,
0xc1c1b071, 0x59590e57, 0x7676e99f, 0xd4d4e135,
0x7878661e, 0x9090b424, 0x3838360e, 0x7979265f,
0x8d8def62, 0x61613859, 0x474795d2, 0x8a8a2aa0,
0x9494b125, 0x8888aa22, 0xf1f18c7d, 0xececd73b,
0x04040501, 0x8484a521, 0xe1e19879, 0x1e1e9b85,
0x535384d7, 0x00000000, 0x19195e47, 0x5d5d0b56,
0x7e7ee39d, 0x4f4f9fd0, 0x9c9cbb27, 0x49491a53,
0x31317c4d, 0xd8d8ee36, 0x08080a02, 0x9f9f7be4,
0x828220a2, 0x1313d4c7, 0x2323e8cb, 0x7a7ae69c,
0xabab42e9, 0xfefe43bd, 0x2a2aa288, 0x4b4b9ad1,
0x01014041, 0x1f1fdbc4, 0xe0e0d838, 0xd6d661b7,
0x8e8e2fa1, 0xdfdf2bf4, 0xcbcb3af1, 0x3b3bf6cd,
0xe7e71dfa, 0x8585e560, 0x54544115, 0x868625a3,
0x838360e3, 0xbaba16ac, 0x7575295c, 0x929234a6,
0x6e6ef799, 0xd0d0e434, 0x6868721a, 0x55550154,
0xb6b619af, 0x4e4edf91, 0xc8c8fa32, 0xc0c0f030,
0xd7d721f6, 0x3232bc8e, 0xc6c675b3, 0x8f8f6fe0,
0x7474691d, 0xdbdb2ef5, 0x8b8b6ae1, 0xb8b8962e,
0x0a0a8a80, 0x9999fe67, 0x2b2be2c9, 0x8181e061,
0x0303c0c3, 0xa4a48d29, 0x8c8caf23, 0xaeae07a9,
0x3434390d, 0x4d4d1f52, 0x3939764f, 0xbdbdd36e,
0x575781d6, 0x6f6fb7d8, 0xdcdceb37, 0x15155144,
0x7b7ba6dd, 0xf7f709fe, 0x3a3ab68c, 0xbcbc932f,
0x0c0c0f03, 0xffff03fc, 0xa9a9c26b, 0xc9c9ba73,
0xb5b5d96c, 0xb1b1dc6d, 0x6d6d375a, 0x45451550,
0x3636b98f, 0x6c6c771b, 0xbebe13ad, 0x4a4ada90,
0xeeee57b9, 0x7777a9de, 0xf2f24cbe, 0xfdfd837e,
0x44445511, 0x6767bdda, 0x71712c5d, 0x05054540,
0x7c7c631f, 0x40405010, 0x6969325b, 0x6363b8db,
0x2828220a, 0x0707c5c2, 0xc4c4f531, 0x2222a88a,
0x969631a7, 0x3737f9ce, 0xeded977a, 0xf6f649bf,
0xb4b4992d, 0xd1d1a475, 0x434390d3, 0x48485a12,
0xe2e258ba, 0x979771e6, 0xd2d264b6, 0xc2c270b2,
0x2626ad8b, 0xa5a5cd68, 0x5e5ecb95, 0x2929624b,
0x30303c0c, 0x5a5ace94, 0xddddab76, 0xf9f9867f,
0x9595f164, 0xe6e65dbb, 0xc7c735f2, 0x24242d09,
0x1717d1c6, 0xb9b9d66f, 0x1b1bdec5, 0x12129486,
0x60607818, 0xc3c330f3, 0xf5f5897c, 0xb3b35cef,
0xe8e8d23a, 0x7373acdf, 0x3535794c, 0x8080a020,
0xe5e59d78, 0xbbbb56ed, 0x7d7d235e, 0xf8f8c63e,
0x5f5f8bd4, 0x2f2fe7c8, 0xe4e4dd39, 0x21216849,
};

static u4 Sbox_T24[256] =
{
0xd55b5b8e, 0x924242d0, 0xeaa7a74d, 0xfdfbfb06,
0xcf3333fc, 0xe2878765, 0x3df4f4c9, 0xb5dede6b,
0x1658584e, 0xb4dada6e, 0x14505044, 0xc10b0bca,
0x28a0a088, 0xf8efef17, 0x2cb0b09c, 0x05141411,
0x2bacac87, 0x669d9dfb, 0x986a6af2, 0x77d9d9ae,
0x2aa8a882, 0xbcfafa46, 0x04101014, 0xc00f0fcf,
0xa8aaaa02, 0x45111154, 0x134c4c5f, 0x269898be,
0x4825256d, 0x841a1a9e, 0x0618181e, 0x9b6666fd,
0x9e7272ec, 0x4309094a, 0x51414110, 0xf7d3d324,
0x934646d5, 0xecbfbf53, 0x9a6262f8, 0x7be9e992,
0x33ccccff, 0x55515104, 0x0b2c2c27, 0x420d0d4f,
0xeeb7b759, 0xcc3f3ff3, 0xaeb2b21c, 0x638989ea,
0xe7939374, 0xb1cece7f, 0x1c70706c, 0xaba6a60d,
0xca2727ed, 0x08202028, 0xeba3a348, 0x975656c1,
0x82020280, 0xdc7f7fa3, 0x965252c4, 0xf9ebeb12,
0x74d5d5a1, 0x8d3e3eb3, 0x3ffcfcc3, 0xa49a9a3e,
0x461d1d5b, 0x071c1c1b, 0xa59e9e3b, 0xfff3f30c,
0xf0cfcf3f, 0x72cdcdbf, 0x175c5c4b, 0xb8eaea52,
0x810e0e8f, 0x5865653d, 0x3cf0f0cc, 0x1964647d,
0xe59b9b7e, 0x87161691, 0x4e3d3d73, 0xaaa2a208,
0x69a1a1c8, 0x6aadadc7, 0x83060685, 0xb0caca7a,
0x70c5c5b5, 0x659191f4, 0xd96b6bb2, 0x892e2ea7,
0xfbe3e318, 0xe8afaf47, 0x0f3c3c33, 0x4a2d2d67,
0x71c1c1b0, 0x5759590e, 0x9f7676e9, 0x35d4d4e1,
0x1e787866, 0x249090b4, 0x0e383836, 0x5f797926,
0x628d8def, 0x59616138, 0xd2474795, 0xa08a8a2a,
0x259494b1, 0x228888aa, 0x7df1f18c, 0x3bececd7,
0x01040405, 0x218484a5, 0x79e1e198, 0x851e1e9b,
0xd7535384, 0x00000000, 0x4719195e, 0x565d5d0b,
0x9d7e7ee3, 0xd04f4f9f, 0x279c9cbb, 0x5349491a,
0x4d31317c, 0x36d8d8ee, 0x0208080a, 0xe49f9f7b,
0xa2828220, 0xc71313d4, 0xcb2323e8, 0x9c7a7ae6,
0xe9abab42, 0xbdfefe43, 0x882a2aa2, 0xd14b4b9a,
0x41010140, 0xc41f1fdb, 0x38e0e0d8, 0xb7d6d661,
0xa18e8e2f, 0xf4dfdf2b, 0xf1cbcb3a, 0xcd3b3bf6,
0xfae7e71d, 0x608585e5, 0x15545441, 0xa3868625,
0xe3838360, 0xacbaba16, 0x5c757529, 0xa6929234,
0x996e6ef7, 0x34d0d0e4, 0x1a686872, 0x54555501,
0xafb6b619, 0x914e4edf, 0x32c8c8fa, 0x30c0c0f0,
0xf6d7d721, 0x8e3232bc, 0xb3c6c675, 0xe08f8f6f,
0x1d747469, 0xf5dbdb2e, 0xe18b8b6a, 0x2eb8b896,
0x800a0a8a, 0x679999fe, 0xc92b2be2, 0x618181e0,
0xc30303c0, 0x29a4a48d, 0x238c8caf, 0xa9aeae07,
0x0d343439, 0x524d4d1f, 0x4f393976, 0x6ebdbdd3,
0xd6575781, 0xd86f6fb7, 0x37dcdceb, 0x44151551,
0xdd7b7ba6, 0xfef7f709, 0x8c3a3ab6, 0x2fbcbc93,
0x030c0c0f, 0xfcffff03, 0x6ba9a9c2, 0x73c9c9ba,
0x6cb5b5d9, 0x6db1b1dc, 0x5a6d6d37, 0x50454515,
0x8f3636b9, 0x1b6c6c77, 0xadbebe13, 0x904a4ada,
0xb9eeee57, 0xde7777a9, 0xbef2f24c, 0x7efdfd83,
0x11444455, 0xda6767bd, 0x5d71712c, 0x40050545,
0x1f7c7c63, 0x10404050, 0x5b696932, 0xdb6363b8,
0x0a282822, 0xc20707c5, 0x31c4c4f5, 0x8a2222a8,
0xa7969631, 0xce3737f9, 0x7aeded97, 0xbff6f649,
0x2db4b499, 0x75d1d1a4, 0xd3434390, 0x1248485a,
0xbae2e258, 0xe6979771, 0xb6d2d264, 0xb2c2c270,
0x8b2626ad, 0x68a5a5cd, 0x955e5ecb, 0x4b292962,
0x0c30303c, 0x945a5ace, 0x76ddddab, 0x7ff9f986,
0x649595f1, 0xbbe6e65d, 0xf2c7c735, 0x0924242d,
0xc61717d1, 0x6fb9b9d6, 0xc51b1bde, 0x86121294,
0x18606078, 0xf3c3c330, 0x7cf5f589, 0xefb3b35c,
0x3ae8e8d2, 0xdf7373ac, 0x4c353579, 0x208080a0,
0x78e5e59d, 0xedbbbb56, 0x5e7d7d23, 0x3ef8f8c6,
0xd45f5f8b, 0xc82f2fe7, 0x39e4e4dd, 0x49212168,
};


/**
* Bit rotation left by a compile-time constant amount
* @param input the input word
* @return input rotated left by ROT bits
*/
template<size_t ROT, typename T>
inline T rotl(T input)
{
	static_assert(ROT > 0 && ROT < 8*sizeof(T), "Invalid rotation constant");
	return static_cast<T>((input << ROT) | (input >> (8*sizeof(T) - ROT)));
}

/**
* Bit rotation right by a compile-time constant amount
* @param input the input word
* @return input rotated right by ROT bits
*/
template<size_t ROT, typename T>
inline T rotr(T input)
{
	static_assert(ROT > 0 && ROT < 8*sizeof(T), "Invalid rotation constant");
	return static_cast<T>((input >> ROT) | (input << (8*sizeof(T) - ROT)));
}

/**
* Byte extraction
* @param byte_num which byte to extract, 0 == highest byte
* @param input the value to extract from
* @return byte byte_num of input
*/
template<typename T> inline u1 get_byte(size_t byte_num, T input)
{
   return static_cast<u1>(
      input >> ((sizeof(T)-1-(byte_num&(sizeof(T)-1))) << 3)
      );
}

/**
* Make a uint32_t from four bytes
* @param i0 the first byte
* @param i1 the second byte
* @param i2 the third byte
* @param i3 the fourth byte
* @return i0 || i1 || i2 || i3
*/
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef uint8_t u1;
typedef uint32_t u4;

inline uint32_t make_uint32(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3)
   {
   return ((static_cast<uint32_t>(i0) << 24) |
           (static_cast<uint32_t>(i1) << 16) |
           (static_cast<uint32_t>(i2) <<  8) |
           (static_cast<uint32_t>(i3)));
   }

/**
* Load a big-endian word
* @param in a pointer to some bytes
* @param off an offset into the array
* @return off'th T of in, as a big-endian value
*/
template<typename T>
inline T load_be(const uint8_t in[], size_t off)
   {
   in += off * sizeof(T);
   T out = 0;
   for(size_t i = 0; i != sizeof(T); ++i)
      out = static_cast<T>((out << 8) | in[i]);
   return out;
   }

/**
* Store two big-endian words
* @param out the output byte array
* @param x0 the first word
* @param x1 the second word
*/
template<typename T>
inline void store_be(uint8_t out[], T x0, T x1)
   {
   store_be(x0, out + (0 * sizeof(T)));
   store_be(x1, out + (1 * sizeof(T)));
   }

/**
* Store a big-endian uint32_t
* @param in the input uint32_t
* @param out the byte array to write to
*/
inline void store_be(uint32_t in, uint8_t out[4])
   {
#if defined(BOTAN_ENDIAN_B2N)
   uint32_t o = BOTAN_ENDIAN_B2N(in);
   std::memcpy(out, &o, sizeof(o));
#else
   out[0] = get_byte(0, in);
   out[1] = get_byte(1, in);
   out[2] = get_byte(2, in);
   out[3] = get_byte(3, in);
#endif
   }

/**
* Store four big-endian words
* @param out the output byte array
* @param x0 the first word
* @param x1 the second word
* @param x2 the third word
* @param x3 the fourth word
*/
template<typename T>
inline void store_be(uint8_t out[], T x0, T x1, T x2, T x3)
   {
   store_be(x0, out + (0 * sizeof(T)));
   store_be(x1, out + (1 * sizeof(T)));
   store_be(x2, out + (2 * sizeof(T)));
   store_be(x3, out + (3 * sizeof(T)));
   }


//AVX related define:
//取t0为输入的第0,16,32,48,64...112个整数开始的4bit数据
static __m256i mask_ffff;
static __m256i vindex_0s;
static __m256i vindex_4i;
static __m256i vindex_swap;
static __m256i vindex_read;
static __m256i mask;
static u4 SBOX32L[256 * 256];
static u4 SBOX32H[256 * 256]; 

#define GET_BLKS(x0, x1, x2, x3, in)					\
	t0 = _mm256_i32gather_epi32((int *)(in+4*0), vindex_4i, 4);	\
	t1 = _mm256_i32gather_epi32((int *)(in+4*1), vindex_4i, 4);	\
	t2 = _mm256_i32gather_epi32((int *)(in+4*2), vindex_4i, 4);	\
	t3 = _mm256_i32gather_epi32((int *)(in+4*3), vindex_4i, 4);	\
	x0 = _mm256_shuffle_epi8(t0, vindex_swap);			\
	x1 = _mm256_shuffle_epi8(t1, vindex_swap);			\
	x2 = _mm256_shuffle_epi8(t2, vindex_swap);			\
	x3 = _mm256_shuffle_epi8(t3, vindex_swap)

#define PUT_BLKS(out, x0, x1, x2, x3)					\
	t0 = _mm256_shuffle_epi8(x0, vindex_swap);			\
	t1 = _mm256_shuffle_epi8(x1, vindex_swap);			\
	t2 = _mm256_shuffle_epi8(x2, vindex_swap);			\
	t3 = _mm256_shuffle_epi8(x3, vindex_swap);			\
	_mm256_storeu_si256((__m256i *)(out+32*0), t0);			\
	_mm256_storeu_si256((__m256i *)(out+32*1), t1);			\
	_mm256_storeu_si256((__m256i *)(out+32*2), t2);			\
	_mm256_storeu_si256((__m256i *)(out+32*3), t3);			\
	x0 = _mm256_i32gather_epi32((int *)(in+32*0), vindex_read, 4);	\
	x1 = _mm256_i32gather_epi32((int *)(in+32*1), vindex_read, 4);	\
	x2 = _mm256_i32gather_epi32((int *)(in+32*2), vindex_read, 4);	\
	x3 = _mm256_i32gather_epi32((int *)(in+32*3), vindex_read, 4);	\
	_mm256_storeu_si256((__m256i *)(out+2*0), x0);			\
	_mm256_storeu_si256((__m256i *)(out+2*1), x1);			\
	_mm256_storeu_si256((__m256i *)(out+2*2), x2);			\
	_mm256_storeu_si256((__m256i *)(out+2*3), x3)

#define S(x0, t0, t1, t2)					\
	t0 = _mm256_and_si256(x0, mask_ffff);			\
	t1 = _mm256_i32gather_epi32((int*)SBOX32L, t0, 4);		\
	t0 = _mm256_srli_epi32(x0, 16);				\
	t2 = _mm256_i32gather_epi32((int*)SBOX32H, t0, 4);		\
	x0 = _mm256_xor_si256(t1, t2)

#define ROT(r0, x0, i, t0, t1)					\
	t0 = _mm256_slli_epi32(x0, i);				\
	t1 = _mm256_srli_epi32(x0,32-i);			\
	r0 = _mm256_xor_si256(t0, t1)

#define L(x0, t0, t1, t2, t3, t4)				\
	ROT(t0, x0,  2, t2, t3);				\
	ROT(t1, x0, 10, t2, t3);				\
	t4 = _mm256_xor_si256(t0, t1);				\
	ROT(t0, x0, 18, t2, t3);				\
	ROT(t1, x0, 24, t2, t3);				\
	t3 = _mm256_xor_si256(t0, t1);				\
	t2 = _mm256_xor_si256(x0, t3);				\
	x0 = _mm256_xor_si256(t2, t4)
#define ROUND(x0, x1, x2, x3, x4, i)				\
	t0 = _mm256_i32gather_epi32((int*)(rk+i), vindex_0s, 4);	\
	t1 = _mm256_xor_si256(x1, x2);				\
	t2 = _mm256_xor_si256(x3, t0);				\
	t0 = _mm256_xor_si256(t1, t2);				\
	S(t0, x4, t1, t2);					\
	L(t0, x4, t1, t2, t3, t4);				\
	x4 = _mm256_xor_si256(x0, t0);

#define ROUNDS(x0, x1, x2, x3, x4)		\
	ROUND(x0, x1, x2, x3, x4, 0);		\
	ROUND(x1, x2, x3, x4, x0, 1);		\
	ROUND(x2, x3, x4, x0, x1, 2);		\
	ROUND(x3, x4, x0, x1, x2, 3);		\
	ROUND(x4, x0, x1, x2, x3, 4);		\
	ROUND(x0, x1, x2, x3, x4, 5);		\
	ROUND(x1, x2, x3, x4, x0, 6);		\
	ROUND(x2, x3, x4, x0, x1, 7);		\
	ROUND(x3, x4, x0, x1, x2, 8);		\
	ROUND(x4, x0, x1, x2, x3, 9);		\
	ROUND(x0, x1, x2, x3, x4, 10);		\
	ROUND(x1, x2, x3, x4, x0, 11);		\
	ROUND(x2, x3, x4, x0, x1, 12);		\
	ROUND(x3, x4, x0, x1, x2, 13);		\
	ROUND(x4, x0, x1, x2, x3, 14);		\
	ROUND(x0, x1, x2, x3, x4, 15);		\
	ROUND(x1, x2, x3, x4, x0, 16);		\
	ROUND(x2, x3, x4, x0, x1, 17);		\
	ROUND(x3, x4, x0, x1, x2, 18);		\
	ROUND(x4, x0, x1, x2, x3, 19);		\
	ROUND(x0, x1, x2, x3, x4, 20);		\
	ROUND(x1, x2, x3, x4, x0, 21);		\
	ROUND(x2, x3, x4, x0, x1, 22);		\
	ROUND(x3, x4, x0, x1, x2, 23);		\
	ROUND(x4, x0, x1, x2, x3, 24);		\
	ROUND(x0, x1, x2, x3, x4, 25);		\
	ROUND(x1, x2, x3, x4, x0, 26);		\
	ROUND(x2, x3, x4, x0, x1, 27);		\
	ROUND(x3, x4, x0, x1, x2, 28);		\
	ROUND(x4, x0, x1, x2, x3, 29);		\
	ROUND(x0, x1, x2, x3, x4, 30);		\
	ROUND(x1, x2, x3, x4, x0, 31)

#define ROUNDS_DEC(x0, x1, x2, x3, x4)		\
	ROUND(x0, x1, x2, x3, x4, 31);		\
	ROUND(x1, x2, x3, x4, x0, 30);		\
	ROUND(x2, x3, x4, x0, x1, 29);		\
	ROUND(x3, x4, x0, x1, x2, 28);		\
	ROUND(x4, x0, x1, x2, x3, 27);		\
	ROUND(x0, x1, x2, x3, x4, 26);		\
	ROUND(x1, x2, x3, x4, x0, 25);		\
	ROUND(x2, x3, x4, x0, x1, 24);		\
	ROUND(x3, x4, x0, x1, x2, 23);		\
	ROUND(x4, x0, x1, x2, x3, 22);		\
	ROUND(x0, x1, x2, x3, x4, 21);		\
	ROUND(x1, x2, x3, x4, x0, 20);		\
	ROUND(x2, x3, x4, x0, x1, 19);		\
	ROUND(x3, x4, x0, x1, x2, 18);		\
	ROUND(x4, x0, x1, x2, x3, 17);		\
	ROUND(x0, x1, x2, x3, x4, 16);		\
	ROUND(x1, x2, x3, x4, x0, 15);		\
	ROUND(x2, x3, x4, x0, x1, 14);		\
	ROUND(x3, x4, x0, x1, x2, 13);		\
	ROUND(x4, x0, x1, x2, x3, 12);		\
	ROUND(x0, x1, x2, x3, x4, 11);		\
	ROUND(x1, x2, x3, x4, x0, 10);		\
	ROUND(x2, x3, x4, x0, x1, 9);		\
	ROUND(x3, x4, x0, x1, x2, 8);		\
	ROUND(x4, x0, x1, x2, x3, 7);		\
	ROUND(x0, x1, x2, x3, x4, 6);		\
	ROUND(x1, x2, x3, x4, x0, 5);		\
	ROUND(x2, x3, x4, x0, x1, 4);		\
	ROUND(x3, x4, x0, x1, x2, 3);		\
	ROUND(x4, x0, x1, x2, x3, 2);		\
	ROUND(x0, x1, x2, x3, x4, 1);		\
	ROUND(x1, x2, x3, x4, x0, 0)
// Import from liarod.h, relying on romangol.h

static void outputChar(const_buf text, size_t len)
{
	forloop( i, 0, len )
	{
		printf("0x%02x, ", text[i]);
		if (i % 16 == 15)
			puts("");
	}
	puts("");
}

static void outputDword(u4 * text, size_t len)
{
	forloop( i, 0, len )
	{
		printf("0x%08x, ", text[i]);
		if (i % 4 == 3)
			puts("");
	}
	puts("");
}
