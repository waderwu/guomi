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

typedef const u1 * const 		const_buf;
typedef u1 *					buf;

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

/*

static uint32_t rol(const uint32_t value, const size_t bits)
{
    return (value << bits) | (value >> (32 - bits));
}


void outputChar(const_buf text, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		printf("0x%02x, ", text[i]);
		if (i % 16 == 15)
			puts("");
	}
	puts("");
}

void outputDword(uint32_t * text, size_t len)
{
	for (size_t i = 0; i < len; ++i)
	{
		printf("0x%08x ", text[i]);
		if (i % 4 == 3)
			puts("");
	}

	puts("");
}
*/



#endif // end of romangol.h