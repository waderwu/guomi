#include "romangol.h"
#include "liarod.h"
#include "sm4fast.h"
#include <omp.h>
#include <time.h>
#define blocknum 8
#define turns 1
/*
inline u4 SM4_T_slow(u4 b)
{
	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);
	// L linear transform
	return t ^ rotl<2>(t) ^ rotl<10>(t) ^ rotl<18>(t) ^ rotl<24>(t);
}
*/

inline u4 SM4_T(u4 b)
{
	return Sbox_T[get_byte(0,b)] ^ (Sbox_T8[get_byte(1,b)]) ^ (Sbox_T16[get_byte(2,b)]) ^ (Sbox_T24[get_byte(3,b)]);
	// return Sbox_T[get_byte(0,b)] ^ rotr<8>(Sbox_T[get_byte(1,b)]) ^ rotr<16>(Sbox_T[get_byte(2,b)]) ^ rotr<24>(Sbox_T[get_byte(3,b)]);
}

// Variant of T for key schedule
inline u4 SM4_Tp(u4 b)
{
	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);

	// L' linear transform
	return t ^ rotl<13>(t) ^ rotl<23>(t);
}

#define SM4_RNDS(k0,k1,k2,k3,F) do {        \
  B0 ^= F(B1 ^ B2 ^ B3 ^ rkey[k0]); \
  B1 ^= F(B0 ^ B2 ^ B3 ^ rkey[k1]); \
  B2 ^= F(B0 ^ B1 ^ B3 ^ rkey[k2]); \
  B3 ^= F(B0 ^ B1 ^ B2 ^ rkey[k3]); \
} while(0)

/*
* SM4 Encryption
*/
void SM4_enc_block(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], const u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
{
	static u4 B0 = load_be<u4>(in, 0);
	static u4 B1 = load_be<u4>(in, 1);
	static u4 B2 = load_be<u4>(in, 2);
	static u4 B3 = load_be<u4>(in, 3);

	// SM4_RNDS( 0,  1,  2,  3, SM4_T_slow);
	SM4_RNDS( 0,  1,  2,  3, SM4_T);
	SM4_RNDS( 4,  5,  6,  7, SM4_T);
	SM4_RNDS( 8,  9, 10, 11, SM4_T);
	SM4_RNDS(12, 13, 14, 15, SM4_T);
	SM4_RNDS(16, 17, 18, 19, SM4_T);
	SM4_RNDS(20, 21, 22, 23, SM4_T);
	SM4_RNDS(24, 25, 26, 27, SM4_T);
	SM4_RNDS(28, 29, 30, 31, SM4_T);
	// SM4_RNDS(28, 29, 30, 31, SM4_T_slow);

	store_be(out, B3, B2, B1, B0);
}

/*
* SM4 Decryption
*/
void SM4_dec_block(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof (u4)])
{
	static u4 B0 = load_be<u4>(in, 0);
	static u4 B1 = load_be<u4>(in, 1);
	static u4 B2 = load_be<u4>(in, 2);
	static u4 B3 = load_be<u4>(in, 3);

	// SM4_RNDS(31, 30, 29, 28, SM4_T_slow);
	SM4_RNDS(31, 30, 29, 28, SM4_T);
	SM4_RNDS(27, 26, 25, 24, SM4_T);
	SM4_RNDS(23, 22, 21, 20, SM4_T);
	SM4_RNDS(19, 18, 17, 16, SM4_T);
	SM4_RNDS(15, 14, 13, 12, SM4_T);
	SM4_RNDS(11, 10,  9,  8, SM4_T);
	SM4_RNDS( 7,  6,  5,  4, SM4_T);
	SM4_RNDS( 3,  2,  1,  0, SM4_T);
	// SM4_RNDS( 3,  2,  1,  0, SM4_T_slow);

	store_be(out, B3, B2, B1, B0);
}

#undef SM4_RNDS

/*
* SM4 Key Schedule
*/
void SM4_key_schedule(const u1 key[SM4_KEY_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
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

	forloop (i,0,32)
	{
		K[i % 4] ^= SM4_Tp(K[(i+1)%4] ^ K[(i+2)%4] ^ K[(i+3)%4] ^ CK[i]);
		rkey[i] = K[i % 4];
	}
}

void stest(int blockid,int blocksize,u1 *p, u1 *c, const u4 *rkey)
{
	clock_t t =clock();
	printf("%d\n", blocksize*blockid);
	for (int j=0; j<turns; j++){
		for (int i=0; i < blocksize; i++)
		{
			SM4_enc_block(p + blocksize*blockid + 16*i, c + blocksize*blockid+ 16 *i, rkey);
		}
	}


	double tt = (double)(clock() - t)/(CLOCKS_PER_SEC*turns);
  double speed =(double) (blocksize*16)/(1024*1024*tt);
	printf("thread: time: %f s\nspeed: %f \n",tt,speed);
}


int main()
{
	// plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// cipher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46
	u1 key[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	u1 tmp[SM4_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

	u1 *p = (u1 *) malloc(sizeof(u1)*16*test_blocks);
	u1 *c = (u1 *) malloc(sizeof(u1)*16*test_blocks);

	for (int i=0; i<test_blocks*16; i++)
  {
    p[i] = tmp[i%16];
  }

	printf("max threads %d\n",omp_get_max_threads());

	clock_t t = clock();
	u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)];
	SM4_key_schedule( key, rkey );
	// outputDword(rkey, 32);

	// printf("Hello, World!, ThreadId=%d\n", omp_get_thread_num() );

	int blocksize = test_blocks/blocknum;
	#pragma omp parallel for
	for (int j=0; j<blocknum; j++)
	{
		// printf("OpenMP Test, 线程编号为: %d\n", omp_get_thread_num());
		stest(j, blocksize, p, c, rkey);
	}
	// for (int j=0 ; j< test_blocks; j++)
	// {
	// 	SM4_enc_block(p + 16*j, c + 16 *j, rkey);
	// }

	double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (turns*test_blocks*16)/(1024*1024*tt);
	printf("time: %f s\nspeed: %f \n",tt,speed);
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <time.h>
// #include <unistd.h>
// #include "romangol.h"
// #include "liarod.h"
// #include "sm4fast.h"
//
// #define SIZE 1073741824
// /*
// inline u4 SM4_T_slow(u4 b)
// {
// 	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);
//
// 	// L linear transform
// 	return t ^ rotl<2>(t) ^ rotl<10>(t) ^ rotl<18>(t) ^ rotl<24>(t);
// }
// */
//
// inline u4 SM4_T(u4 b)
// {
// 	return Sbox_T[get_byte(0,b)] ^ (Sbox_T8[get_byte(1,b)]) ^ (Sbox_T16[get_byte(2,b)]) ^ (Sbox_T24[get_byte(3,b)]);
// 	// return Sbox_T[get_byte(0,b)] ^ rotr<8>(Sbox_T[get_byte(1,b)]) ^ rotr<16>(Sbox_T[get_byte(2,b)]) ^ rotr<24>(Sbox_T[get_byte(3,b)]);
// }
//
// // Variant of T for key schedule
// inline u4 SM4_Tp(u4 b)
// {
// 	const u4 t = make_uint32(Sbox[get_byte(0,b)], Sbox[get_byte(1,b)], Sbox[get_byte(2,b)], Sbox[get_byte(3,b)]);
//
// 	// L' linear transform
// 	return t ^ rotl<13>(t) ^ rotl<23>(t);
// }
//
// #define SM4_RNDS(k0,k1,k2,k3,F) do {        \
//   B0 ^= F(B1 ^ B2 ^ B3 ^ rkey[k0]); \
//   B1 ^= F(B0 ^ B2 ^ B3 ^ rkey[k1]); \
//   B2 ^= F(B0 ^ B1 ^ B3 ^ rkey[k2]); \
//   B3 ^= F(B0 ^ B1 ^ B2 ^ rkey[k3]); \
// } while(0)
//
// /*
// * SM4 Encryption
// */
// void SM4_enc_block(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], const u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
// {
// 	static u4 B0 = load_be<u4>(in, 0);
// 	static u4 B1 = load_be<u4>(in, 1);
// 	static u4 B2 = load_be<u4>(in, 2);
// 	static u4 B3 = load_be<u4>(in, 3);;
//
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
//
// 	store_be(out, B3, B2, B1, B0);
// }
//
// /*
// * SM4 Decryption
// */
// void SM4_dec_block(const u1 in[SM4_BLOCK_SIZE], u1 out[SM4_BLOCK_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof (u4)])
// {
// 	static u4 B0 = load_be<u4>(in, 0);
// 	static u4 B1 = load_be<u4>(in, 1);
// 	static u4 B2 = load_be<u4>(in, 2);
// 	static u4 B3 = load_be<u4>(in, 3);
//
// 	// SM4_RNDS(31, 30, 29, 28, SM4_T_slow);
// 	SM4_RNDS(31, 30, 29, 28, SM4_T);
// 	SM4_RNDS(27, 26, 25, 24, SM4_T);
// 	SM4_RNDS(23, 22, 21, 20, SM4_T);
// 	SM4_RNDS(19, 18, 17, 16, SM4_T);
// 	SM4_RNDS(15, 14, 13, 12, SM4_T);
// 	SM4_RNDS(11, 10,  9,  8, SM4_T);
// 	SM4_RNDS( 7,  6,  5,  4, SM4_T);
// 	SM4_RNDS( 3,  2,  1,  0, SM4_T);
// 	// SM4_RNDS( 3,  2,  1,  0, SM4_T_slow);
//
// 	store_be(out, B3, B2, B1, B0);
// }
//
// #undef SM4_RNDS
//
// /*
// * SM4 Key Schedule
// */
// void SM4_key_schedule(const u1 key[SM4_KEY_SIZE], u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)])
// {
// 	// System parameter or family key
// 	const u4 FK[4] = { 0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc };
//
// 	const u4 CK[32] = {
// 	0x00070E15, 0x1C232A31, 0x383F464D, 0x545B6269,
// 	0x70777E85, 0x8C939AA1, 0xA8AFB6BD, 0xC4CBD2D9,
// 	0xE0E7EEF5, 0xFC030A11, 0x181F262D, 0x343B4249,
// 	0x50575E65, 0x6C737A81, 0x888F969D, 0xA4ABB2B9,
// 	0xC0C7CED5, 0xDCE3EAF1, 0xF8FF060D, 0x141B2229,
// 	0x30373E45, 0x4C535A61, 0x686F767D, 0x848B9299,
// 	0xA0A7AEB5, 0xBCC3CAD1, 0xD8DFE6ED, 0xF4FB0209,
// 	0x10171E25, 0x2C333A41, 0x484F565D, 0x646B7279
// 	};
//
// 	u4 K[4];
//
// 	K[0] = load_be<u4>(key, 0) ^ FK[0];
// 	K[1] = load_be<u4>(key, 1) ^ FK[1];
// 	K[2] = load_be<u4>(key, 2) ^ FK[2];
// 	K[3] = load_be<u4>(key, 3) ^ FK[3];
//
// 	forloop (i,0,32)
// 	{
// 		K[i % 4] ^= SM4_Tp(K[(i+1)%4] ^ K[(i+2)%4] ^ K[(i+3)%4] ^ CK[i]);
// 		rkey[i] = K[i % 4];
// 	}
// }
//
// int main(int argc, char *argv[])
// {
// 	// plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// 	// key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// 	// cipher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46
// 	u1 key[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
// 	u1 p[SM4_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
// 	u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)];
// 	SM4_key_schedule( key, rkey );
// 	outputDword(rkey, 32);
//
// 	// char * filename = argv[1];
// 	// u1 *buf =(u1 *)malloc(1024);
// 	u1 *out = (u1 *) malloc(1024);
// 	// int fd = open(filename,O_RDONLY);
// 	// if (!fd){
// 	// 	printf("%s\n", "open file error");
// 	// 	exit(-1);
// 	// }
//
// 	int haveread;
// 	clock_t t = clock();
// 		for(int i=0; i < 10; i++)
// 		{
// 			SM4_enc_block(p,out+i*16, rkey);
// 			outputDword(rkey, 32);
// 			outputChar(out,160);
// 			outputChar(p, 16);
// 		}
//   // while ((haveread=read(fd,buf,1024)) > 0)
//   // {
// 	// 	printf("%d\n", haveread);
// 	// 	outputChar(buf, haveread);
// 	// 	printf("%d\n", haveread/16);
// 	// 	for(int i=0; i < 10; i++)
// 	// 	{
// 	// 		SM4_enc_block(p,out+i*16, rkey );
// 	// 		outputDword(rkey, 32);
// 	// 		outputChar(out,160);
// 	// 		outputChar(p, 16);
// 	// 	}
//   //
//   //
//   // }
//
//
//
//
//
// 	// SM4_enc_block( p, c, rkey );
// 	// // should be: 0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46,
// 	// printf("The encrypted data:\n");
// 	// outputChar(c, sizeof(c));
//   //
// 	// SM4_dec_block( c, c, rkey );
// 	// printf("The original data:\n");
// 	// outputChar(c, sizeof(c));
//
// 	// return 0;
//
//
//
//
// 	printf("time: %ld ms\n", clock() - t );
// }
