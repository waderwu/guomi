#include<cstdio>
#include <cuda.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef uint8_t u1;
// typedef uint16_t u2;
typedef uint32_t u4;
// typedef uint64_t u8;


#define SM4_BLOCK_SIZE 16
#define SM4_KEY_SIZE 16
#define SM4_RND_KEY_SIZE 128
#define test_blocks (1 <<25)
#define SIZE (2<<28)


__constant__ const u1 Sbox[256] =
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

#define forloop(i, start, end) for ( size_t (i) = (start); (i) < (end); ++(i) )

#define SM4_RNDS(k0,k1,k2,k3,F,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24) do {        \
  B0 ^= F(B1 ^ B2 ^ B3 ^ rkey[k0],Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24); \
  B1 ^= F(B0 ^ B2 ^ B3 ^ rkey[k1],Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24); \
  B2 ^= F(B0 ^ B1 ^ B3 ^ rkey[k2],Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24); \
  B3 ^= F(B0 ^ B1 ^ B2 ^ rkey[k3],Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24); \
} while(0)


template<typename T>
__device__ __forceinline__ u1 get_byte(size_t byte_num, T input)
{
   return static_cast<u1>(
      input >> ((sizeof(T)-1-(byte_num&(sizeof(T)-1))) << 3)
      );
}


template<typename T>
__device__ __forceinline__ T load_be(const uint8_t in[], size_t off)
   {
   in += off * sizeof(T);
   T out = 0;
   for(size_t i = 0; i != sizeof(T); ++i)
      out = static_cast<T>((out << 8) | in[i]);
   return out;
   }


__device__ __forceinline__ void store_be(uint32_t in, uint8_t out[4])
   {
   out[0] = get_byte(0, in);
   out[1] = get_byte(1, in);
   out[2] = get_byte(2, in);
   out[3] = get_byte(3, in);
 	}


template<typename T>
__device__ __forceinline__ void store_be(uint8_t out[], T x0, T x1)
   {
   store_be(x0, out + (0 * sizeof(T)));
   store_be(x1, out + (1 * sizeof(T)));
   }

template<typename T>
__device__ __forceinline__ void store_be(uint8_t out[], T x0, T x1, T x2, T x3)
   {
   store_be(x0, out + (0 * sizeof(T)));
   store_be(x1, out + (1 * sizeof(T)));
   store_be(x2, out + (2 * sizeof(T)));
   store_be(x3, out + (3 * sizeof(T)));
   }

template<size_t ROT, typename T>
__device__ __forceinline__ T rotl(T input)
{
	// static_assert(ROT > 0 && ROT < 8*sizeof(T), "Invalid rotation constant");
	return static_cast<T>((input << ROT) | (input >> (8*sizeof(T) - ROT)));
}



__device__ __forceinline__ uint32_t make_uint32(uint8_t i0, uint8_t i1, uint8_t i2, uint8_t i3)
   {
   return ((static_cast<uint32_t>(i0) << 24) |
           (static_cast<uint32_t>(i1) << 16) |
           (static_cast<uint32_t>(i2) <<  8) |
           (static_cast<uint32_t>(i3)));
   }


// Variant of T for key schedule
__device__ __forceinline__ u4 SM4_Tp(u4 b)
{
	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);

	// L' linear transform
	return t ^ rotl<13>(t) ^ rotl<23>(t);
}


__device__ __forceinline__ u4 SM4_T(u4 b,const u4 *Sbox_T,const u4 *Sbox_T8,const u4 *Sbox_T16,const u4 *Sbox_T24)
{
	return Sbox_T[get_byte(0,b)] ^ (Sbox_T8[get_byte(1,b)]) ^ (Sbox_T16[get_byte(2,b)]) ^ (Sbox_T24[get_byte(3,b)]);
	// return Sbox_T[get_byte(0,b)] ^ rotr<8>(Sbox_T[get_byte(1,b)]) ^ rotr<16>(Sbox_T[get_byte(2,b)]) ^ rotr<24>(Sbox_T[get_byte(3,b)]);
}


//key explansion

__global__ void SM4_key_schedule(const u1 key[SM4_KEY_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
{
	// System parameter or family key
	const u4 FK[4] = { 0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc };

	const u4 CK[32] = {
	0x00070E15, 0x1C232A31, 0x383F464D, 0x545B6269,
	0x70777E85, 0x8C939AA1, 0xA8AFB6BD, 0xC4CBD2D9,
	0xE0E7EEF5, 0xFC030A11, 0x181F262D, 0x343B4249,
	0x50575E65, 0x6C737A81, 0x888F969D, 0xA4ABB2B9,
	0xC0C7CED5, 0xDCE3EAF1, 0xF8FF060D, 0x141B2229,
	0x30373E45, 0x4C535A61, 0x686F767D, 0x848B9299,
	0xA0A7AEB5, 0xBCC3CAD1, 0xD8DFE6ED, 0xF4FB0209,
	0x10171E25, 0x2C333A41, 0x484F565D, 0x646B7279
	};

	u4 K[4];

	K[0] = load_be<u4>(key, 0) ^ FK[0];
	K[1] = load_be<u4>(key, 1) ^ FK[1];
	K[2] = load_be<u4>(key, 2) ^ FK[2];
	K[3] = load_be<u4>(key, 3) ^ FK[3];

	forloop (i, 0, 32)
	{
		K[i % 4] ^= SM4_Tp(K[(i+1)%4] ^ K[(i+2)%4] ^ K[(i+3)%4] ^ CK[i]);
		rkey[i] = K[i % 4];
	}
}



static void outputChar(u1 * text, size_t len)
{
	forloop( i, 0, len )
	{
		printf("0x%02x, ", text[i]);
		if (i % 16 == 15)
			puts("");
	}
	puts("");
}
__device__ void SM4_init(u4 *Sbox_T, u4 *Sbox_T8,u4 *Sbox_T16,u4 *Sbox_T24)
{
Sbox_T[0]=0x8ed55b5b;
Sbox_T8[0]=0x5b8ed55b;
Sbox_T16[0]=0x5b5b8ed5;
Sbox_T24[0]=0xd55b5b8e;
Sbox_T[1]=0xd0924242;
Sbox_T8[1]=0x42d09242;
Sbox_T16[1]=0x4242d092;
Sbox_T24[1]=0x924242d0;
Sbox_T[2]=0x4deaa7a7;
Sbox_T8[2]=0xa74deaa7;
Sbox_T16[2]=0xa7a74dea;
Sbox_T24[2]=0xeaa7a74d;
Sbox_T[3]=0x6fdfbfb;
Sbox_T8[3]=0xfb06fdfb;
Sbox_T16[3]=0xfbfb06fd;
Sbox_T24[3]=0xfdfbfb06;
Sbox_T[4]=0xfccf3333;
Sbox_T8[4]=0x33fccf33;
Sbox_T16[4]=0x3333fccf;
Sbox_T24[4]=0xcf3333fc;
Sbox_T[5]=0x65e28787;
Sbox_T8[5]=0x8765e287;
Sbox_T16[5]=0x878765e2;
Sbox_T24[5]=0xe2878765;
Sbox_T[6]=0xc93df4f4;
Sbox_T8[6]=0xf4c93df4;
Sbox_T16[6]=0xf4f4c93d;
Sbox_T24[6]=0x3df4f4c9;
Sbox_T[7]=0x6bb5dede;
Sbox_T8[7]=0xde6bb5de;
Sbox_T16[7]=0xdede6bb5;
Sbox_T24[7]=0xb5dede6b;
Sbox_T[8]=0x4e165858;
Sbox_T8[8]=0x584e1658;
Sbox_T16[8]=0x58584e16;
Sbox_T24[8]=0x1658584e;
Sbox_T[9]=0x6eb4dada;
Sbox_T8[9]=0xda6eb4da;
Sbox_T16[9]=0xdada6eb4;
Sbox_T24[9]=0xb4dada6e;
Sbox_T[10]=0x44145050;
Sbox_T8[10]=0x50441450;
Sbox_T16[10]=0x50504414;
Sbox_T24[10]=0x14505044;
Sbox_T[11]=0xcac10b0b;
Sbox_T8[11]=0xbcac10b;
Sbox_T16[11]=0xb0bcac1;
Sbox_T24[11]=0xc10b0bca;
Sbox_T[12]=0x8828a0a0;
Sbox_T8[12]=0xa08828a0;
Sbox_T16[12]=0xa0a08828;
Sbox_T24[12]=0x28a0a088;
Sbox_T[13]=0x17f8efef;
Sbox_T8[13]=0xef17f8ef;
Sbox_T16[13]=0xefef17f8;
Sbox_T24[13]=0xf8efef17;
Sbox_T[14]=0x9c2cb0b0;
Sbox_T8[14]=0xb09c2cb0;
Sbox_T16[14]=0xb0b09c2c;
Sbox_T24[14]=0x2cb0b09c;
Sbox_T[15]=0x11051414;
Sbox_T8[15]=0x14110514;
Sbox_T16[15]=0x14141105;
Sbox_T24[15]=0x5141411;
Sbox_T[16]=0x872bacac;
Sbox_T8[16]=0xac872bac;
Sbox_T16[16]=0xacac872b;
Sbox_T24[16]=0x2bacac87;
Sbox_T[17]=0xfb669d9d;
Sbox_T8[17]=0x9dfb669d;
Sbox_T16[17]=0x9d9dfb66;
Sbox_T24[17]=0x669d9dfb;
Sbox_T[18]=0xf2986a6a;
Sbox_T8[18]=0x6af2986a;
Sbox_T16[18]=0x6a6af298;
Sbox_T24[18]=0x986a6af2;
Sbox_T[19]=0xae77d9d9;
Sbox_T8[19]=0xd9ae77d9;
Sbox_T16[19]=0xd9d9ae77;
Sbox_T24[19]=0x77d9d9ae;
Sbox_T[20]=0x822aa8a8;
Sbox_T8[20]=0xa8822aa8;
Sbox_T16[20]=0xa8a8822a;
Sbox_T24[20]=0x2aa8a882;
Sbox_T[21]=0x46bcfafa;
Sbox_T8[21]=0xfa46bcfa;
Sbox_T16[21]=0xfafa46bc;
Sbox_T24[21]=0xbcfafa46;
Sbox_T[22]=0x14041010;
Sbox_T8[22]=0x10140410;
Sbox_T16[22]=0x10101404;
Sbox_T24[22]=0x4101014;
Sbox_T[23]=0xcfc00f0f;
Sbox_T8[23]=0xfcfc00f;
Sbox_T16[23]=0xf0fcfc0;
Sbox_T24[23]=0xc00f0fcf;
Sbox_T[24]=0x2a8aaaa;
Sbox_T8[24]=0xaa02a8aa;
Sbox_T16[24]=0xaaaa02a8;
Sbox_T24[24]=0xa8aaaa02;
Sbox_T[25]=0x54451111;
Sbox_T8[25]=0x11544511;
Sbox_T16[25]=0x11115445;
Sbox_T24[25]=0x45111154;
Sbox_T[26]=0x5f134c4c;
Sbox_T8[26]=0x4c5f134c;
Sbox_T16[26]=0x4c4c5f13;
Sbox_T24[26]=0x134c4c5f;
Sbox_T[27]=0xbe269898;
Sbox_T8[27]=0x98be2698;
Sbox_T16[27]=0x9898be26;
Sbox_T24[27]=0x269898be;
Sbox_T[28]=0x6d482525;
Sbox_T8[28]=0x256d4825;
Sbox_T16[28]=0x25256d48;
Sbox_T24[28]=0x4825256d;
Sbox_T[29]=0x9e841a1a;
Sbox_T8[29]=0x1a9e841a;
Sbox_T16[29]=0x1a1a9e84;
Sbox_T24[29]=0x841a1a9e;
Sbox_T[30]=0x1e061818;
Sbox_T8[30]=0x181e0618;
Sbox_T16[30]=0x18181e06;
Sbox_T24[30]=0x618181e;
Sbox_T[31]=0xfd9b6666;
Sbox_T8[31]=0x66fd9b66;
Sbox_T16[31]=0x6666fd9b;
Sbox_T24[31]=0x9b6666fd;
Sbox_T[32]=0xec9e7272;
Sbox_T8[32]=0x72ec9e72;
Sbox_T16[32]=0x7272ec9e;
Sbox_T24[32]=0x9e7272ec;
Sbox_T[33]=0x4a430909;
Sbox_T8[33]=0x94a4309;
Sbox_T16[33]=0x9094a43;
Sbox_T24[33]=0x4309094a;
Sbox_T[34]=0x10514141;
Sbox_T8[34]=0x41105141;
Sbox_T16[34]=0x41411051;
Sbox_T24[34]=0x51414110;
Sbox_T[35]=0x24f7d3d3;
Sbox_T8[35]=0xd324f7d3;
Sbox_T16[35]=0xd3d324f7;
Sbox_T24[35]=0xf7d3d324;
Sbox_T[36]=0xd5934646;
Sbox_T8[36]=0x46d59346;
Sbox_T16[36]=0x4646d593;
Sbox_T24[36]=0x934646d5;
Sbox_T[37]=0x53ecbfbf;
Sbox_T8[37]=0xbf53ecbf;
Sbox_T16[37]=0xbfbf53ec;
Sbox_T24[37]=0xecbfbf53;
Sbox_T[38]=0xf89a6262;
Sbox_T8[38]=0x62f89a62;
Sbox_T16[38]=0x6262f89a;
Sbox_T24[38]=0x9a6262f8;
Sbox_T[39]=0x927be9e9;
Sbox_T8[39]=0xe9927be9;
Sbox_T16[39]=0xe9e9927b;
Sbox_T24[39]=0x7be9e992;
Sbox_T[40]=0xff33cccc;
Sbox_T8[40]=0xccff33cc;
Sbox_T16[40]=0xccccff33;
Sbox_T24[40]=0x33ccccff;
Sbox_T[41]=0x4555151;
Sbox_T8[41]=0x51045551;
Sbox_T16[41]=0x51510455;
Sbox_T24[41]=0x55515104;
Sbox_T[42]=0x270b2c2c;
Sbox_T8[42]=0x2c270b2c;
Sbox_T16[42]=0x2c2c270b;
Sbox_T24[42]=0xb2c2c27;
Sbox_T[43]=0x4f420d0d;
Sbox_T8[43]=0xd4f420d;
Sbox_T16[43]=0xd0d4f42;
Sbox_T24[43]=0x420d0d4f;
Sbox_T[44]=0x59eeb7b7;
Sbox_T8[44]=0xb759eeb7;
Sbox_T16[44]=0xb7b759ee;
Sbox_T24[44]=0xeeb7b759;
Sbox_T[45]=0xf3cc3f3f;
Sbox_T8[45]=0x3ff3cc3f;
Sbox_T16[45]=0x3f3ff3cc;
Sbox_T24[45]=0xcc3f3ff3;
Sbox_T[46]=0x1caeb2b2;
Sbox_T8[46]=0xb21caeb2;
Sbox_T16[46]=0xb2b21cae;
Sbox_T24[46]=0xaeb2b21c;
Sbox_T[47]=0xea638989;
Sbox_T8[47]=0x89ea6389;
Sbox_T16[47]=0x8989ea63;
Sbox_T24[47]=0x638989ea;
Sbox_T[48]=0x74e79393;
Sbox_T8[48]=0x9374e793;
Sbox_T16[48]=0x939374e7;
Sbox_T24[48]=0xe7939374;
Sbox_T[49]=0x7fb1cece;
Sbox_T8[49]=0xce7fb1ce;
Sbox_T16[49]=0xcece7fb1;
Sbox_T24[49]=0xb1cece7f;
Sbox_T[50]=0x6c1c7070;
Sbox_T8[50]=0x706c1c70;
Sbox_T16[50]=0x70706c1c;
Sbox_T24[50]=0x1c70706c;
Sbox_T[51]=0xdaba6a6;
Sbox_T8[51]=0xa60daba6;
Sbox_T16[51]=0xa6a60dab;
Sbox_T24[51]=0xaba6a60d;
Sbox_T[52]=0xedca2727;
Sbox_T8[52]=0x27edca27;
Sbox_T16[52]=0x2727edca;
Sbox_T24[52]=0xca2727ed;
Sbox_T[53]=0x28082020;
Sbox_T8[53]=0x20280820;
Sbox_T16[53]=0x20202808;
Sbox_T24[53]=0x8202028;
Sbox_T[54]=0x48eba3a3;
Sbox_T8[54]=0xa348eba3;
Sbox_T16[54]=0xa3a348eb;
Sbox_T24[54]=0xeba3a348;
Sbox_T[55]=0xc1975656;
Sbox_T8[55]=0x56c19756;
Sbox_T16[55]=0x5656c197;
Sbox_T24[55]=0x975656c1;
Sbox_T[56]=0x80820202;
Sbox_T8[56]=0x2808202;
Sbox_T16[56]=0x2028082;
Sbox_T24[56]=0x82020280;
Sbox_T[57]=0xa3dc7f7f;
Sbox_T8[57]=0x7fa3dc7f;
Sbox_T16[57]=0x7f7fa3dc;
Sbox_T24[57]=0xdc7f7fa3;
Sbox_T[58]=0xc4965252;
Sbox_T8[58]=0x52c49652;
Sbox_T16[58]=0x5252c496;
Sbox_T24[58]=0x965252c4;
Sbox_T[59]=0x12f9ebeb;
Sbox_T8[59]=0xeb12f9eb;
Sbox_T16[59]=0xebeb12f9;
Sbox_T24[59]=0xf9ebeb12;
Sbox_T[60]=0xa174d5d5;
Sbox_T8[60]=0xd5a174d5;
Sbox_T16[60]=0xd5d5a174;
Sbox_T24[60]=0x74d5d5a1;
Sbox_T[61]=0xb38d3e3e;
Sbox_T8[61]=0x3eb38d3e;
Sbox_T16[61]=0x3e3eb38d;
Sbox_T24[61]=0x8d3e3eb3;
Sbox_T[62]=0xc33ffcfc;
Sbox_T8[62]=0xfcc33ffc;
Sbox_T16[62]=0xfcfcc33f;
Sbox_T24[62]=0x3ffcfcc3;
Sbox_T[63]=0x3ea49a9a;
Sbox_T8[63]=0x9a3ea49a;
Sbox_T16[63]=0x9a9a3ea4;
Sbox_T24[63]=0xa49a9a3e;
Sbox_T[64]=0x5b461d1d;
Sbox_T8[64]=0x1d5b461d;
Sbox_T16[64]=0x1d1d5b46;
Sbox_T24[64]=0x461d1d5b;
Sbox_T[65]=0x1b071c1c;
Sbox_T8[65]=0x1c1b071c;
Sbox_T16[65]=0x1c1c1b07;
Sbox_T24[65]=0x71c1c1b;
Sbox_T[66]=0x3ba59e9e;
Sbox_T8[66]=0x9e3ba59e;
Sbox_T16[66]=0x9e9e3ba5;
Sbox_T24[66]=0xa59e9e3b;
Sbox_T[67]=0xcfff3f3;
Sbox_T8[67]=0xf30cfff3;
Sbox_T16[67]=0xf3f30cff;
Sbox_T24[67]=0xfff3f30c;
Sbox_T[68]=0x3ff0cfcf;
Sbox_T8[68]=0xcf3ff0cf;
Sbox_T16[68]=0xcfcf3ff0;
Sbox_T24[68]=0xf0cfcf3f;
Sbox_T[69]=0xbf72cdcd;
Sbox_T8[69]=0xcdbf72cd;
Sbox_T16[69]=0xcdcdbf72;
Sbox_T24[69]=0x72cdcdbf;
Sbox_T[70]=0x4b175c5c;
Sbox_T8[70]=0x5c4b175c;
Sbox_T16[70]=0x5c5c4b17;
Sbox_T24[70]=0x175c5c4b;
Sbox_T[71]=0x52b8eaea;
Sbox_T8[71]=0xea52b8ea;
Sbox_T16[71]=0xeaea52b8;
Sbox_T24[71]=0xb8eaea52;
Sbox_T[72]=0x8f810e0e;
Sbox_T8[72]=0xe8f810e;
Sbox_T16[72]=0xe0e8f81;
Sbox_T24[72]=0x810e0e8f;
Sbox_T[73]=0x3d586565;
Sbox_T8[73]=0x653d5865;
Sbox_T16[73]=0x65653d58;
Sbox_T24[73]=0x5865653d;
Sbox_T[74]=0xcc3cf0f0;
Sbox_T8[74]=0xf0cc3cf0;
Sbox_T16[74]=0xf0f0cc3c;
Sbox_T24[74]=0x3cf0f0cc;
Sbox_T[75]=0x7d196464;
Sbox_T8[75]=0x647d1964;
Sbox_T16[75]=0x64647d19;
Sbox_T24[75]=0x1964647d;
Sbox_T[76]=0x7ee59b9b;
Sbox_T8[76]=0x9b7ee59b;
Sbox_T16[76]=0x9b9b7ee5;
Sbox_T24[76]=0xe59b9b7e;
Sbox_T[77]=0x91871616;
Sbox_T8[77]=0x16918716;
Sbox_T16[77]=0x16169187;
Sbox_T24[77]=0x87161691;
Sbox_T[78]=0x734e3d3d;
Sbox_T8[78]=0x3d734e3d;
Sbox_T16[78]=0x3d3d734e;
Sbox_T24[78]=0x4e3d3d73;
Sbox_T[79]=0x8aaa2a2;
Sbox_T8[79]=0xa208aaa2;
Sbox_T16[79]=0xa2a208aa;
Sbox_T24[79]=0xaaa2a208;
Sbox_T[80]=0xc869a1a1;
Sbox_T8[80]=0xa1c869a1;
Sbox_T16[80]=0xa1a1c869;
Sbox_T24[80]=0x69a1a1c8;
Sbox_T[81]=0xc76aadad;
Sbox_T8[81]=0xadc76aad;
Sbox_T16[81]=0xadadc76a;
Sbox_T24[81]=0x6aadadc7;
Sbox_T[82]=0x85830606;
Sbox_T8[82]=0x6858306;
Sbox_T16[82]=0x6068583;
Sbox_T24[82]=0x83060685;
Sbox_T[83]=0x7ab0caca;
Sbox_T8[83]=0xca7ab0ca;
Sbox_T16[83]=0xcaca7ab0;
Sbox_T24[83]=0xb0caca7a;
Sbox_T[84]=0xb570c5c5;
Sbox_T8[84]=0xc5b570c5;
Sbox_T16[84]=0xc5c5b570;
Sbox_T24[84]=0x70c5c5b5;
Sbox_T[85]=0xf4659191;
Sbox_T8[85]=0x91f46591;
Sbox_T16[85]=0x9191f465;
Sbox_T24[85]=0x659191f4;
Sbox_T[86]=0xb2d96b6b;
Sbox_T8[86]=0x6bb2d96b;
Sbox_T16[86]=0x6b6bb2d9;
Sbox_T24[86]=0xd96b6bb2;
Sbox_T[87]=0xa7892e2e;
Sbox_T8[87]=0x2ea7892e;
Sbox_T16[87]=0x2e2ea789;
Sbox_T24[87]=0x892e2ea7;
Sbox_T[88]=0x18fbe3e3;
Sbox_T8[88]=0xe318fbe3;
Sbox_T16[88]=0xe3e318fb;
Sbox_T24[88]=0xfbe3e318;
Sbox_T[89]=0x47e8afaf;
Sbox_T8[89]=0xaf47e8af;
Sbox_T16[89]=0xafaf47e8;
Sbox_T24[89]=0xe8afaf47;
Sbox_T[90]=0x330f3c3c;
Sbox_T8[90]=0x3c330f3c;
Sbox_T16[90]=0x3c3c330f;
Sbox_T24[90]=0xf3c3c33;
Sbox_T[91]=0x674a2d2d;
Sbox_T8[91]=0x2d674a2d;
Sbox_T16[91]=0x2d2d674a;
Sbox_T24[91]=0x4a2d2d67;
Sbox_T[92]=0xb071c1c1;
Sbox_T8[92]=0xc1b071c1;
Sbox_T16[92]=0xc1c1b071;
Sbox_T24[92]=0x71c1c1b0;
Sbox_T[93]=0xe575959;
Sbox_T8[93]=0x590e5759;
Sbox_T16[93]=0x59590e57;
Sbox_T24[93]=0x5759590e;
Sbox_T[94]=0xe99f7676;
Sbox_T8[94]=0x76e99f76;
Sbox_T16[94]=0x7676e99f;
Sbox_T24[94]=0x9f7676e9;
Sbox_T[95]=0xe135d4d4;
Sbox_T8[95]=0xd4e135d4;
Sbox_T16[95]=0xd4d4e135;
Sbox_T24[95]=0x35d4d4e1;
Sbox_T[96]=0x661e7878;
Sbox_T8[96]=0x78661e78;
Sbox_T16[96]=0x7878661e;
Sbox_T24[96]=0x1e787866;
Sbox_T[97]=0xb4249090;
Sbox_T8[97]=0x90b42490;
Sbox_T16[97]=0x9090b424;
Sbox_T24[97]=0x249090b4;
Sbox_T[98]=0x360e3838;
Sbox_T8[98]=0x38360e38;
Sbox_T16[98]=0x3838360e;
Sbox_T24[98]=0xe383836;
Sbox_T[99]=0x265f7979;
Sbox_T8[99]=0x79265f79;
Sbox_T16[99]=0x7979265f;
Sbox_T24[99]=0x5f797926;
Sbox_T[100]=0xef628d8d;
Sbox_T8[100]=0x8def628d;
Sbox_T16[100]=0x8d8def62;
Sbox_T24[100]=0x628d8def;
Sbox_T[101]=0x38596161;
Sbox_T8[101]=0x61385961;
Sbox_T16[101]=0x61613859;
Sbox_T24[101]=0x59616138;
Sbox_T[102]=0x95d24747;
Sbox_T8[102]=0x4795d247;
Sbox_T16[102]=0x474795d2;
Sbox_T24[102]=0xd2474795;
Sbox_T[103]=0x2aa08a8a;
Sbox_T8[103]=0x8a2aa08a;
Sbox_T16[103]=0x8a8a2aa0;
Sbox_T24[103]=0xa08a8a2a;
Sbox_T[104]=0xb1259494;
Sbox_T8[104]=0x94b12594;
Sbox_T16[104]=0x9494b125;
Sbox_T24[104]=0x259494b1;
Sbox_T[105]=0xaa228888;
Sbox_T8[105]=0x88aa2288;
Sbox_T16[105]=0x8888aa22;
Sbox_T24[105]=0x228888aa;
Sbox_T[106]=0x8c7df1f1;
Sbox_T8[106]=0xf18c7df1;
Sbox_T16[106]=0xf1f18c7d;
Sbox_T24[106]=0x7df1f18c;
Sbox_T[107]=0xd73becec;
Sbox_T8[107]=0xecd73bec;
Sbox_T16[107]=0xececd73b;
Sbox_T24[107]=0x3bececd7;
Sbox_T[108]=0x5010404;
Sbox_T8[108]=0x4050104;
Sbox_T16[108]=0x4040501;
Sbox_T24[108]=0x1040405;
Sbox_T[109]=0xa5218484;
Sbox_T8[109]=0x84a52184;
Sbox_T16[109]=0x8484a521;
Sbox_T24[109]=0x218484a5;
Sbox_T[110]=0x9879e1e1;
Sbox_T8[110]=0xe19879e1;
Sbox_T16[110]=0xe1e19879;
Sbox_T24[110]=0x79e1e198;
Sbox_T[111]=0x9b851e1e;
Sbox_T8[111]=0x1e9b851e;
Sbox_T16[111]=0x1e1e9b85;
Sbox_T24[111]=0x851e1e9b;
Sbox_T[112]=0x84d75353;
Sbox_T8[112]=0x5384d753;
Sbox_T16[112]=0x535384d7;
Sbox_T24[112]=0xd7535384;
Sbox_T[113]=0x0;
Sbox_T8[113]=0x0;
Sbox_T16[113]=0x0;
Sbox_T24[113]=0x0;
Sbox_T[114]=0x5e471919;
Sbox_T8[114]=0x195e4719;
Sbox_T16[114]=0x19195e47;
Sbox_T24[114]=0x4719195e;
Sbox_T[115]=0xb565d5d;
Sbox_T8[115]=0x5d0b565d;
Sbox_T16[115]=0x5d5d0b56;
Sbox_T24[115]=0x565d5d0b;
Sbox_T[116]=0xe39d7e7e;
Sbox_T8[116]=0x7ee39d7e;
Sbox_T16[116]=0x7e7ee39d;
Sbox_T24[116]=0x9d7e7ee3;
Sbox_T[117]=0x9fd04f4f;
Sbox_T8[117]=0x4f9fd04f;
Sbox_T16[117]=0x4f4f9fd0;
Sbox_T24[117]=0xd04f4f9f;
Sbox_T[118]=0xbb279c9c;
Sbox_T8[118]=0x9cbb279c;
Sbox_T16[118]=0x9c9cbb27;
Sbox_T24[118]=0x279c9cbb;
Sbox_T[119]=0x1a534949;
Sbox_T8[119]=0x491a5349;
Sbox_T16[119]=0x49491a53;
Sbox_T24[119]=0x5349491a;
Sbox_T[120]=0x7c4d3131;
Sbox_T8[120]=0x317c4d31;
Sbox_T16[120]=0x31317c4d;
Sbox_T24[120]=0x4d31317c;
Sbox_T[121]=0xee36d8d8;
Sbox_T8[121]=0xd8ee36d8;
Sbox_T16[121]=0xd8d8ee36;
Sbox_T24[121]=0x36d8d8ee;
Sbox_T[122]=0xa020808;
Sbox_T8[122]=0x80a0208;
Sbox_T16[122]=0x8080a02;
Sbox_T24[122]=0x208080a;
Sbox_T[123]=0x7be49f9f;
Sbox_T8[123]=0x9f7be49f;
Sbox_T16[123]=0x9f9f7be4;
Sbox_T24[123]=0xe49f9f7b;
Sbox_T[124]=0x20a28282;
Sbox_T8[124]=0x8220a282;
Sbox_T16[124]=0x828220a2;
Sbox_T24[124]=0xa2828220;
Sbox_T[125]=0xd4c71313;
Sbox_T8[125]=0x13d4c713;
Sbox_T16[125]=0x1313d4c7;
Sbox_T24[125]=0xc71313d4;
Sbox_T[126]=0xe8cb2323;
Sbox_T8[126]=0x23e8cb23;
Sbox_T16[126]=0x2323e8cb;
Sbox_T24[126]=0xcb2323e8;
Sbox_T[127]=0xe69c7a7a;
Sbox_T8[127]=0x7ae69c7a;
Sbox_T16[127]=0x7a7ae69c;
Sbox_T24[127]=0x9c7a7ae6;
Sbox_T[128]=0x42e9abab;
Sbox_T8[128]=0xab42e9ab;
Sbox_T16[128]=0xabab42e9;
Sbox_T24[128]=0xe9abab42;
Sbox_T[129]=0x43bdfefe;
Sbox_T8[129]=0xfe43bdfe;
Sbox_T16[129]=0xfefe43bd;
Sbox_T24[129]=0xbdfefe43;
Sbox_T[130]=0xa2882a2a;
Sbox_T8[130]=0x2aa2882a;
Sbox_T16[130]=0x2a2aa288;
Sbox_T24[130]=0x882a2aa2;
Sbox_T[131]=0x9ad14b4b;
Sbox_T8[131]=0x4b9ad14b;
Sbox_T16[131]=0x4b4b9ad1;
Sbox_T24[131]=0xd14b4b9a;
Sbox_T[132]=0x40410101;
Sbox_T8[132]=0x1404101;
Sbox_T16[132]=0x1014041;
Sbox_T24[132]=0x41010140;
Sbox_T[133]=0xdbc41f1f;
Sbox_T8[133]=0x1fdbc41f;
Sbox_T16[133]=0x1f1fdbc4;
Sbox_T24[133]=0xc41f1fdb;
Sbox_T[134]=0xd838e0e0;
Sbox_T8[134]=0xe0d838e0;
Sbox_T16[134]=0xe0e0d838;
Sbox_T24[134]=0x38e0e0d8;
Sbox_T[135]=0x61b7d6d6;
Sbox_T8[135]=0xd661b7d6;
Sbox_T16[135]=0xd6d661b7;
Sbox_T24[135]=0xb7d6d661;
Sbox_T[136]=0x2fa18e8e;
Sbox_T8[136]=0x8e2fa18e;
Sbox_T16[136]=0x8e8e2fa1;
Sbox_T24[136]=0xa18e8e2f;
Sbox_T[137]=0x2bf4dfdf;
Sbox_T8[137]=0xdf2bf4df;
Sbox_T16[137]=0xdfdf2bf4;
Sbox_T24[137]=0xf4dfdf2b;
Sbox_T[138]=0x3af1cbcb;
Sbox_T8[138]=0xcb3af1cb;
Sbox_T16[138]=0xcbcb3af1;
Sbox_T24[138]=0xf1cbcb3a;
Sbox_T[139]=0xf6cd3b3b;
Sbox_T8[139]=0x3bf6cd3b;
Sbox_T16[139]=0x3b3bf6cd;
Sbox_T24[139]=0xcd3b3bf6;
Sbox_T[140]=0x1dfae7e7;
Sbox_T8[140]=0xe71dfae7;
Sbox_T16[140]=0xe7e71dfa;
Sbox_T24[140]=0xfae7e71d;
Sbox_T[141]=0xe5608585;
Sbox_T8[141]=0x85e56085;
Sbox_T16[141]=0x8585e560;
Sbox_T24[141]=0x608585e5;
Sbox_T[142]=0x41155454;
Sbox_T8[142]=0x54411554;
Sbox_T16[142]=0x54544115;
Sbox_T24[142]=0x15545441;
Sbox_T[143]=0x25a38686;
Sbox_T8[143]=0x8625a386;
Sbox_T16[143]=0x868625a3;
Sbox_T24[143]=0xa3868625;
Sbox_T[144]=0x60e38383;
Sbox_T8[144]=0x8360e383;
Sbox_T16[144]=0x838360e3;
Sbox_T24[144]=0xe3838360;
Sbox_T[145]=0x16acbaba;
Sbox_T8[145]=0xba16acba;
Sbox_T16[145]=0xbaba16ac;
Sbox_T24[145]=0xacbaba16;
Sbox_T[146]=0x295c7575;
Sbox_T8[146]=0x75295c75;
Sbox_T16[146]=0x7575295c;
Sbox_T24[146]=0x5c757529;
Sbox_T[147]=0x34a69292;
Sbox_T8[147]=0x9234a692;
Sbox_T16[147]=0x929234a6;
Sbox_T24[147]=0xa6929234;
Sbox_T[148]=0xf7996e6e;
Sbox_T8[148]=0x6ef7996e;
Sbox_T16[148]=0x6e6ef799;
Sbox_T24[148]=0x996e6ef7;
Sbox_T[149]=0xe434d0d0;
Sbox_T8[149]=0xd0e434d0;
Sbox_T16[149]=0xd0d0e434;
Sbox_T24[149]=0x34d0d0e4;
Sbox_T[150]=0x721a6868;
Sbox_T8[150]=0x68721a68;
Sbox_T16[150]=0x6868721a;
Sbox_T24[150]=0x1a686872;
Sbox_T[151]=0x1545555;
Sbox_T8[151]=0x55015455;
Sbox_T16[151]=0x55550154;
Sbox_T24[151]=0x54555501;
Sbox_T[152]=0x19afb6b6;
Sbox_T8[152]=0xb619afb6;
Sbox_T16[152]=0xb6b619af;
Sbox_T24[152]=0xafb6b619;
Sbox_T[153]=0xdf914e4e;
Sbox_T8[153]=0x4edf914e;
Sbox_T16[153]=0x4e4edf91;
Sbox_T24[153]=0x914e4edf;
Sbox_T[154]=0xfa32c8c8;
Sbox_T8[154]=0xc8fa32c8;
Sbox_T16[154]=0xc8c8fa32;
Sbox_T24[154]=0x32c8c8fa;
Sbox_T[155]=0xf030c0c0;
Sbox_T8[155]=0xc0f030c0;
Sbox_T16[155]=0xc0c0f030;
Sbox_T24[155]=0x30c0c0f0;
Sbox_T[156]=0x21f6d7d7;
Sbox_T8[156]=0xd721f6d7;
Sbox_T16[156]=0xd7d721f6;
Sbox_T24[156]=0xf6d7d721;
Sbox_T[157]=0xbc8e3232;
Sbox_T8[157]=0x32bc8e32;
Sbox_T16[157]=0x3232bc8e;
Sbox_T24[157]=0x8e3232bc;
Sbox_T[158]=0x75b3c6c6;
Sbox_T8[158]=0xc675b3c6;
Sbox_T16[158]=0xc6c675b3;
Sbox_T24[158]=0xb3c6c675;
Sbox_T[159]=0x6fe08f8f;
Sbox_T8[159]=0x8f6fe08f;
Sbox_T16[159]=0x8f8f6fe0;
Sbox_T24[159]=0xe08f8f6f;
Sbox_T[160]=0x691d7474;
Sbox_T8[160]=0x74691d74;
Sbox_T16[160]=0x7474691d;
Sbox_T24[160]=0x1d747469;
Sbox_T[161]=0x2ef5dbdb;
Sbox_T8[161]=0xdb2ef5db;
Sbox_T16[161]=0xdbdb2ef5;
Sbox_T24[161]=0xf5dbdb2e;
Sbox_T[162]=0x6ae18b8b;
Sbox_T8[162]=0x8b6ae18b;
Sbox_T16[162]=0x8b8b6ae1;
Sbox_T24[162]=0xe18b8b6a;
Sbox_T[163]=0x962eb8b8;
Sbox_T8[163]=0xb8962eb8;
Sbox_T16[163]=0xb8b8962e;
Sbox_T24[163]=0x2eb8b896;
Sbox_T[164]=0x8a800a0a;
Sbox_T8[164]=0xa8a800a;
Sbox_T16[164]=0xa0a8a80;
Sbox_T24[164]=0x800a0a8a;
Sbox_T[165]=0xfe679999;
Sbox_T8[165]=0x99fe6799;
Sbox_T16[165]=0x9999fe67;
Sbox_T24[165]=0x679999fe;
Sbox_T[166]=0xe2c92b2b;
Sbox_T8[166]=0x2be2c92b;
Sbox_T16[166]=0x2b2be2c9;
Sbox_T24[166]=0xc92b2be2;
Sbox_T[167]=0xe0618181;
Sbox_T8[167]=0x81e06181;
Sbox_T16[167]=0x8181e061;
Sbox_T24[167]=0x618181e0;
Sbox_T[168]=0xc0c30303;
Sbox_T8[168]=0x3c0c303;
Sbox_T16[168]=0x303c0c3;
Sbox_T24[168]=0xc30303c0;
Sbox_T[169]=0x8d29a4a4;
Sbox_T8[169]=0xa48d29a4;
Sbox_T16[169]=0xa4a48d29;
Sbox_T24[169]=0x29a4a48d;
Sbox_T[170]=0xaf238c8c;
Sbox_T8[170]=0x8caf238c;
Sbox_T16[170]=0x8c8caf23;
Sbox_T24[170]=0x238c8caf;
Sbox_T[171]=0x7a9aeae;
Sbox_T8[171]=0xae07a9ae;
Sbox_T16[171]=0xaeae07a9;
Sbox_T24[171]=0xa9aeae07;
Sbox_T[172]=0x390d3434;
Sbox_T8[172]=0x34390d34;
Sbox_T16[172]=0x3434390d;
Sbox_T24[172]=0xd343439;
Sbox_T[173]=0x1f524d4d;
Sbox_T8[173]=0x4d1f524d;
Sbox_T16[173]=0x4d4d1f52;
Sbox_T24[173]=0x524d4d1f;
Sbox_T[174]=0x764f3939;
Sbox_T8[174]=0x39764f39;
Sbox_T16[174]=0x3939764f;
Sbox_T24[174]=0x4f393976;
Sbox_T[175]=0xd36ebdbd;
Sbox_T8[175]=0xbdd36ebd;
Sbox_T16[175]=0xbdbdd36e;
Sbox_T24[175]=0x6ebdbdd3;
Sbox_T[176]=0x81d65757;
Sbox_T8[176]=0x5781d657;
Sbox_T16[176]=0x575781d6;
Sbox_T24[176]=0xd6575781;
Sbox_T[177]=0xb7d86f6f;
Sbox_T8[177]=0x6fb7d86f;
Sbox_T16[177]=0x6f6fb7d8;
Sbox_T24[177]=0xd86f6fb7;
Sbox_T[178]=0xeb37dcdc;
Sbox_T8[178]=0xdceb37dc;
Sbox_T16[178]=0xdcdceb37;
Sbox_T24[178]=0x37dcdceb;
Sbox_T[179]=0x51441515;
Sbox_T8[179]=0x15514415;
Sbox_T16[179]=0x15155144;
Sbox_T24[179]=0x44151551;
Sbox_T[180]=0xa6dd7b7b;
Sbox_T8[180]=0x7ba6dd7b;
Sbox_T16[180]=0x7b7ba6dd;
Sbox_T24[180]=0xdd7b7ba6;
Sbox_T[181]=0x9fef7f7;
Sbox_T8[181]=0xf709fef7;
Sbox_T16[181]=0xf7f709fe;
Sbox_T24[181]=0xfef7f709;
Sbox_T[182]=0xb68c3a3a;
Sbox_T8[182]=0x3ab68c3a;
Sbox_T16[182]=0x3a3ab68c;
Sbox_T24[182]=0x8c3a3ab6;
Sbox_T[183]=0x932fbcbc;
Sbox_T8[183]=0xbc932fbc;
Sbox_T16[183]=0xbcbc932f;
Sbox_T24[183]=0x2fbcbc93;
Sbox_T[184]=0xf030c0c;
Sbox_T8[184]=0xc0f030c;
Sbox_T16[184]=0xc0c0f03;
Sbox_T24[184]=0x30c0c0f;
Sbox_T[185]=0x3fcffff;
Sbox_T8[185]=0xff03fcff;
Sbox_T16[185]=0xffff03fc;
Sbox_T24[185]=0xfcffff03;
Sbox_T[186]=0xc26ba9a9;
Sbox_T8[186]=0xa9c26ba9;
Sbox_T16[186]=0xa9a9c26b;
Sbox_T24[186]=0x6ba9a9c2;
Sbox_T[187]=0xba73c9c9;
Sbox_T8[187]=0xc9ba73c9;
Sbox_T16[187]=0xc9c9ba73;
Sbox_T24[187]=0x73c9c9ba;
Sbox_T[188]=0xd96cb5b5;
Sbox_T8[188]=0xb5d96cb5;
Sbox_T16[188]=0xb5b5d96c;
Sbox_T24[188]=0x6cb5b5d9;
Sbox_T[189]=0xdc6db1b1;
Sbox_T8[189]=0xb1dc6db1;
Sbox_T16[189]=0xb1b1dc6d;
Sbox_T24[189]=0x6db1b1dc;
Sbox_T[190]=0x375a6d6d;
Sbox_T8[190]=0x6d375a6d;
Sbox_T16[190]=0x6d6d375a;
Sbox_T24[190]=0x5a6d6d37;
Sbox_T[191]=0x15504545;
Sbox_T8[191]=0x45155045;
Sbox_T16[191]=0x45451550;
Sbox_T24[191]=0x50454515;
Sbox_T[192]=0xb98f3636;
Sbox_T8[192]=0x36b98f36;
Sbox_T16[192]=0x3636b98f;
Sbox_T24[192]=0x8f3636b9;
Sbox_T[193]=0x771b6c6c;
Sbox_T8[193]=0x6c771b6c;
Sbox_T16[193]=0x6c6c771b;
Sbox_T24[193]=0x1b6c6c77;
Sbox_T[194]=0x13adbebe;
Sbox_T8[194]=0xbe13adbe;
Sbox_T16[194]=0xbebe13ad;
Sbox_T24[194]=0xadbebe13;
Sbox_T[195]=0xda904a4a;
Sbox_T8[195]=0x4ada904a;
Sbox_T16[195]=0x4a4ada90;
Sbox_T24[195]=0x904a4ada;
Sbox_T[196]=0x57b9eeee;
Sbox_T8[196]=0xee57b9ee;
Sbox_T16[196]=0xeeee57b9;
Sbox_T24[196]=0xb9eeee57;
Sbox_T[197]=0xa9de7777;
Sbox_T8[197]=0x77a9de77;
Sbox_T16[197]=0x7777a9de;
Sbox_T24[197]=0xde7777a9;
Sbox_T[198]=0x4cbef2f2;
Sbox_T8[198]=0xf24cbef2;
Sbox_T16[198]=0xf2f24cbe;
Sbox_T24[198]=0xbef2f24c;
Sbox_T[199]=0x837efdfd;
Sbox_T8[199]=0xfd837efd;
Sbox_T16[199]=0xfdfd837e;
Sbox_T24[199]=0x7efdfd83;
Sbox_T[200]=0x55114444;
Sbox_T8[200]=0x44551144;
Sbox_T16[200]=0x44445511;
Sbox_T24[200]=0x11444455;
Sbox_T[201]=0xbdda6767;
Sbox_T8[201]=0x67bdda67;
Sbox_T16[201]=0x6767bdda;
Sbox_T24[201]=0xda6767bd;
Sbox_T[202]=0x2c5d7171;
Sbox_T8[202]=0x712c5d71;
Sbox_T16[202]=0x71712c5d;
Sbox_T24[202]=0x5d71712c;
Sbox_T[203]=0x45400505;
Sbox_T8[203]=0x5454005;
Sbox_T16[203]=0x5054540;
Sbox_T24[203]=0x40050545;
Sbox_T[204]=0x631f7c7c;
Sbox_T8[204]=0x7c631f7c;
Sbox_T16[204]=0x7c7c631f;
Sbox_T24[204]=0x1f7c7c63;
Sbox_T[205]=0x50104040;
Sbox_T8[205]=0x40501040;
Sbox_T16[205]=0x40405010;
Sbox_T24[205]=0x10404050;
Sbox_T[206]=0x325b6969;
Sbox_T8[206]=0x69325b69;
Sbox_T16[206]=0x6969325b;
Sbox_T24[206]=0x5b696932;
Sbox_T[207]=0xb8db6363;
Sbox_T8[207]=0x63b8db63;
Sbox_T16[207]=0x6363b8db;
Sbox_T24[207]=0xdb6363b8;
Sbox_T[208]=0x220a2828;
Sbox_T8[208]=0x28220a28;
Sbox_T16[208]=0x2828220a;
Sbox_T24[208]=0xa282822;
Sbox_T[209]=0xc5c20707;
Sbox_T8[209]=0x7c5c207;
Sbox_T16[209]=0x707c5c2;
Sbox_T24[209]=0xc20707c5;
Sbox_T[210]=0xf531c4c4;
Sbox_T8[210]=0xc4f531c4;
Sbox_T16[210]=0xc4c4f531;
Sbox_T24[210]=0x31c4c4f5;
Sbox_T[211]=0xa88a2222;
Sbox_T8[211]=0x22a88a22;
Sbox_T16[211]=0x2222a88a;
Sbox_T24[211]=0x8a2222a8;
Sbox_T[212]=0x31a79696;
Sbox_T8[212]=0x9631a796;
Sbox_T16[212]=0x969631a7;
Sbox_T24[212]=0xa7969631;
Sbox_T[213]=0xf9ce3737;
Sbox_T8[213]=0x37f9ce37;
Sbox_T16[213]=0x3737f9ce;
Sbox_T24[213]=0xce3737f9;
Sbox_T[214]=0x977aeded;
Sbox_T8[214]=0xed977aed;
Sbox_T16[214]=0xeded977a;
Sbox_T24[214]=0x7aeded97;
Sbox_T[215]=0x49bff6f6;
Sbox_T8[215]=0xf649bff6;
Sbox_T16[215]=0xf6f649bf;
Sbox_T24[215]=0xbff6f649;
Sbox_T[216]=0x992db4b4;
Sbox_T8[216]=0xb4992db4;
Sbox_T16[216]=0xb4b4992d;
Sbox_T24[216]=0x2db4b499;
Sbox_T[217]=0xa475d1d1;
Sbox_T8[217]=0xd1a475d1;
Sbox_T16[217]=0xd1d1a475;
Sbox_T24[217]=0x75d1d1a4;
Sbox_T[218]=0x90d34343;
Sbox_T8[218]=0x4390d343;
Sbox_T16[218]=0x434390d3;
Sbox_T24[218]=0xd3434390;
Sbox_T[219]=0x5a124848;
Sbox_T8[219]=0x485a1248;
Sbox_T16[219]=0x48485a12;
Sbox_T24[219]=0x1248485a;
Sbox_T[220]=0x58bae2e2;
Sbox_T8[220]=0xe258bae2;
Sbox_T16[220]=0xe2e258ba;
Sbox_T24[220]=0xbae2e258;
Sbox_T[221]=0x71e69797;
Sbox_T8[221]=0x9771e697;
Sbox_T16[221]=0x979771e6;
Sbox_T24[221]=0xe6979771;
Sbox_T[222]=0x64b6d2d2;
Sbox_T8[222]=0xd264b6d2;
Sbox_T16[222]=0xd2d264b6;
Sbox_T24[222]=0xb6d2d264;
Sbox_T[223]=0x70b2c2c2;
Sbox_T8[223]=0xc270b2c2;
Sbox_T16[223]=0xc2c270b2;
Sbox_T24[223]=0xb2c2c270;
Sbox_T[224]=0xad8b2626;
Sbox_T8[224]=0x26ad8b26;
Sbox_T16[224]=0x2626ad8b;
Sbox_T24[224]=0x8b2626ad;
Sbox_T[225]=0xcd68a5a5;
Sbox_T8[225]=0xa5cd68a5;
Sbox_T16[225]=0xa5a5cd68;
Sbox_T24[225]=0x68a5a5cd;
Sbox_T[226]=0xcb955e5e;
Sbox_T8[226]=0x5ecb955e;
Sbox_T16[226]=0x5e5ecb95;
Sbox_T24[226]=0x955e5ecb;
Sbox_T[227]=0x624b2929;
Sbox_T8[227]=0x29624b29;
Sbox_T16[227]=0x2929624b;
Sbox_T24[227]=0x4b292962;
Sbox_T[228]=0x3c0c3030;
Sbox_T8[228]=0x303c0c30;
Sbox_T16[228]=0x30303c0c;
Sbox_T24[228]=0xc30303c;
Sbox_T[229]=0xce945a5a;
Sbox_T8[229]=0x5ace945a;
Sbox_T16[229]=0x5a5ace94;
Sbox_T24[229]=0x945a5ace;
Sbox_T[230]=0xab76dddd;
Sbox_T8[230]=0xddab76dd;
Sbox_T16[230]=0xddddab76;
Sbox_T24[230]=0x76ddddab;
Sbox_T[231]=0x867ff9f9;
Sbox_T8[231]=0xf9867ff9;
Sbox_T16[231]=0xf9f9867f;
Sbox_T24[231]=0x7ff9f986;
Sbox_T[232]=0xf1649595;
Sbox_T8[232]=0x95f16495;
Sbox_T16[232]=0x9595f164;
Sbox_T24[232]=0x649595f1;
Sbox_T[233]=0x5dbbe6e6;
Sbox_T8[233]=0xe65dbbe6;
Sbox_T16[233]=0xe6e65dbb;
Sbox_T24[233]=0xbbe6e65d;
Sbox_T[234]=0x35f2c7c7;
Sbox_T8[234]=0xc735f2c7;
Sbox_T16[234]=0xc7c735f2;
Sbox_T24[234]=0xf2c7c735;
Sbox_T[235]=0x2d092424;
Sbox_T8[235]=0x242d0924;
Sbox_T16[235]=0x24242d09;
Sbox_T24[235]=0x924242d;
Sbox_T[236]=0xd1c61717;
Sbox_T8[236]=0x17d1c617;
Sbox_T16[236]=0x1717d1c6;
Sbox_T24[236]=0xc61717d1;
Sbox_T[237]=0xd66fb9b9;
Sbox_T8[237]=0xb9d66fb9;
Sbox_T16[237]=0xb9b9d66f;
Sbox_T24[237]=0x6fb9b9d6;
Sbox_T[238]=0xdec51b1b;
Sbox_T8[238]=0x1bdec51b;
Sbox_T16[238]=0x1b1bdec5;
Sbox_T24[238]=0xc51b1bde;
Sbox_T[239]=0x94861212;
Sbox_T8[239]=0x12948612;
Sbox_T16[239]=0x12129486;
Sbox_T24[239]=0x86121294;
Sbox_T[240]=0x78186060;
Sbox_T8[240]=0x60781860;
Sbox_T16[240]=0x60607818;
Sbox_T24[240]=0x18606078;
Sbox_T[241]=0x30f3c3c3;
Sbox_T8[241]=0xc330f3c3;
Sbox_T16[241]=0xc3c330f3;
Sbox_T24[241]=0xf3c3c330;
Sbox_T[242]=0x897cf5f5;
Sbox_T8[242]=0xf5897cf5;
Sbox_T16[242]=0xf5f5897c;
Sbox_T24[242]=0x7cf5f589;
Sbox_T[243]=0x5cefb3b3;
Sbox_T8[243]=0xb35cefb3;
Sbox_T16[243]=0xb3b35cef;
Sbox_T24[243]=0xefb3b35c;
Sbox_T[244]=0xd23ae8e8;
Sbox_T8[244]=0xe8d23ae8;
Sbox_T16[244]=0xe8e8d23a;
Sbox_T24[244]=0x3ae8e8d2;
Sbox_T[245]=0xacdf7373;
Sbox_T8[245]=0x73acdf73;
Sbox_T16[245]=0x7373acdf;
Sbox_T24[245]=0xdf7373ac;
Sbox_T[246]=0x794c3535;
Sbox_T8[246]=0x35794c35;
Sbox_T16[246]=0x3535794c;
Sbox_T24[246]=0x4c353579;
Sbox_T[247]=0xa0208080;
Sbox_T8[247]=0x80a02080;
Sbox_T16[247]=0x8080a020;
Sbox_T24[247]=0x208080a0;
Sbox_T[248]=0x9d78e5e5;
Sbox_T8[248]=0xe59d78e5;
Sbox_T16[248]=0xe5e59d78;
Sbox_T24[248]=0x78e5e59d;
Sbox_T[249]=0x56edbbbb;
Sbox_T8[249]=0xbb56edbb;
Sbox_T16[249]=0xbbbb56ed;
Sbox_T24[249]=0xedbbbb56;
Sbox_T[250]=0x235e7d7d;
Sbox_T8[250]=0x7d235e7d;
Sbox_T16[250]=0x7d7d235e;
Sbox_T24[250]=0x5e7d7d23;
Sbox_T[251]=0xc63ef8f8;
Sbox_T8[251]=0xf8c63ef8;
Sbox_T16[251]=0xf8f8c63e;
Sbox_T24[251]=0x3ef8f8c6;
Sbox_T[252]=0x8bd45f5f;
Sbox_T8[252]=0x5f8bd45f;
Sbox_T16[252]=0x5f5f8bd4;
Sbox_T24[252]=0xd45f5f8b;
Sbox_T[253]=0xe7c82f2f;
Sbox_T8[253]=0x2fe7c82f;
Sbox_T16[253]=0x2f2fe7c8;
Sbox_T24[253]=0xc82f2fe7;
Sbox_T[254]=0xdd39e4e4;
Sbox_T8[254]=0xe4dd39e4;
Sbox_T16[254]=0xe4e4dd39;
Sbox_T24[254]=0x39e4e4dd;
Sbox_T[255]=0x68492121;
Sbox_T8[255]=0x21684921;
Sbox_T16[255]=0x21216849;
Sbox_T24[255]=0x49212168;
}

__device__ void SM4_init__(u4 *Sbox_T, u4 *Sbox_T8,u4 *Sbox_T16,u4 *Sbox_T24)
{
  u4 T[256] = {
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

u4 T8[256] =
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

u4 T16[256] =
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

u4 T24[256] =
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

  for (int i=0; i<256; i++){
    Sbox_T[i] = T[i];
    Sbox_T8[i] = T8[i];
    Sbox_T16[i] = T16[i];
    Sbox_T24[i] = T24[i];
  }

}

// __device__ void SM4_enc_block_cuda(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], const u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
// {
//   u4 B0 = load_be<u4>(in, 0);
//   u4 B1 = load_be<u4>(in, 1);
//   u4 B2 = load_be<u4>(in, 2);
//   u4 B3 = load_be<u4>(in, 3);

//   // SM4_RNDS( 0,  1,  2,  3, SM4_T_slow);
//   SM4_RNDS( 0,  1,  2,  3, SM4_T);
//   SM4_RNDS( 4,  5,  6,  7, SM4_T);
//   SM4_RNDS( 8,  9, 10, 11, SM4_T);
//   SM4_RNDS(12, 13, 14, 15, SM4_T);
//   SM4_RNDS(16, 17, 18, 19, SM4_T);
//   SM4_RNDS(20, 21, 22, 23, SM4_T);
//   SM4_RNDS(24, 25, 26, 27, SM4_T);
//   SM4_RNDS(28, 29, 30, 31, SM4_T);
//   // SM4_RNDS(28, 29, 30, 31, SM4_T_slow);

//   store_be(out, B3, B2, B1, B0);
// }






// __global__ void SM4_enc_block(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], const u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
// {
// 	u4 B0 = load_be<u4>(in, 0);
// 	u4 B1 = load_be<u4>(in, 1);
// 	u4 B2 = load_be<u4>(in, 2);
// 	u4 B3 = load_be<u4>(in, 3);

// 	// SM4_RNDS( 0,  1,  2,  3, SM4_T_slow);
// 	SM4_RNDS( 0,  1,  2,  3, SM4_T);
// 	SM4_RNDS( 4,  5,  6,  7, SM4_T);
// 	SM4_RNDS( 8,  9, 10, 11, SM4_T);
// 	SM4_RNDS(12, 13, 14, 15, SM4_T);
// 	SM4_RNDS(16, 17, 18, 19, SM4_T);
// 	SM4_RNDS(20, 21, 22, 23, SM4_T);
// 	SM4_RNDS(24, 25, 26, 27, SM4_T);
// 	SM4_RNDS(28, 29, 30, 31, SM4_T);
// 	// SM4_RNDS(28, 29, 30, 31, SM4_T_slow);

// 	store_be(out, B3, B2, B1, B0);
// }

// __global__ void speed_test(int n, const u1 *in, u1 *out, const u4 *rkey)
// {
//   int index = blockIdx.x*blockDim.x+threadIdx.x;
//   int stride = blockDim.x*gridDim.x;

//   for (int i=index; i<n; i +=stride)
//     SM4_enc_block_cuda(in, out+16*i,rkey);
// }

__global__ void speed_test_share(int n, const u1 *in, u1 *out, const u4 *rkey)
{
  int index = blockIdx.x*blockDim.x+threadIdx.x;
  int stride = blockDim.x*gridDim.x;

  __shared__ u4 Sbox_T[256];
  __shared__ u4 Sbox_T8[256];
  __shared__ u4 Sbox_T16[256];
  __shared__ u4 Sbox_T24[256];

  if (threadIdx.x == 0){
    SM4_init(Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
  }

  __syncthreads();

  for (int i=index; i<n; i +=stride)
  {
      u4 B0 = load_be<u4>(in+16*i, 0);
      u4 B1 = load_be<u4>(in+16*i, 1);
      u4 B2 = load_be<u4>(in+16*i, 2);
      u4 B3 = load_be<u4>(in+16*i, 3);

      // SM4_RNDS( 0,  1,  2,  3, SM4_T_slow);
      SM4_RNDS( 0,  1,  2,  3, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS( 4,  5,  6,  7, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS( 8,  9, 10, 11, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS(12, 13, 14, 15, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS(16, 17, 18, 19, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS(20, 21, 22, 23, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS(24, 25, 26, 27, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      SM4_RNDS(28, 29, 30, 31, SM4_T,Sbox_T,Sbox_T8,Sbox_T16,Sbox_T24);
      // SM4_RNDS(28, 29, 30, 31, SM4_T_slow);

      store_be(out+16*i, B3, B2, B1, B0);
  }

}

void printDeviceProp(const cudaDeviceProp &prop)
{
    printf("Device Name : %s.\n", prop.name);
    printf("totalGlobalMem : %d.\n", prop.totalGlobalMem);
    printf("sharedMemPerBlock : %d.\n", prop.sharedMemPerBlock);
    printf("regsPerBlock : %d.\n", prop.regsPerBlock);
    printf("warpSize : %d.\n", prop.warpSize);
    printf("memPitch : %d.\n", prop.memPitch);
    printf("maxThreadsPerBlock : %d.\n", prop.maxThreadsPerBlock);
    printf("maxThreadsDim[0 - 2] : %d %d %d.\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    printf("maxGridSize[0 - 2] : %d %d %d.\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
    printf("totalConstMem : %d.\n", prop.totalConstMem);
    printf("major.minor : %d.%d.\n", prop.major, prop.minor);
    printf("clockRate : %d.\n", prop.clockRate);
    printf("textureAlignment : %d.\n", prop.textureAlignment);
    printf("deviceOverlap : %d.\n", prop.deviceOverlap);
    printf("multiProcessorCount : %d.\n", prop.multiProcessorCount);
}



bool InitCUDA()
{
    int count;

    //取得支持Cuda的装置的数目
    cudaGetDeviceCount(&count);

    if (count == 0) {
        fprintf(stderr, "There is no device.\n");
        return false;
    }

    int i;

    for (i = 0; i < count; i++) {

        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        //打印设备信息
        printDeviceProp(prop);

        if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
            if (prop.major >= 1) {
                break;
            }
        }
    }

    if (i == count) {
        fprintf(stderr, "There is no device supporting CUDA 1.x.\n");
        return false;
    }

    cudaSetDevice(i);

    return true;
}
