#include <string.h>
#include "sm3.h"

void sm3_init(sm3_ctx_t *ctx)
{
	ctx->digest[0] = 0x7380166F;
	ctx->digest[1] = 0x4914B2B9;
	ctx->digest[2] = 0x172442D7;
	ctx->digest[3] = 0xDA8A0600;
	ctx->digest[4] = 0xA96F30BC;
	ctx->digest[5] = 0x163138AA;
	ctx->digest[6] = 0xE38DEE4D;
	ctx->digest[7] = 0xB0FB0E4E;

	ctx->nblocks = 0;
	ctx->num = 0;
}

void sm3_update(sm3_ctx_t *ctx, const unsigned char* data, size_t data_len)
{
	if (ctx->num) {
		unsigned int left = SM3_BLOCK_SIZE - ctx->num;
		if (data_len < left) {
			memcpy(ctx->block + ctx->num, data, data_len);
			ctx->num += data_len;
			return;
		} else {
			memcpy(ctx->block + ctx->num, data, left);
			sm3_compress(ctx->digest, ctx->block);
			ctx->nblocks++;
			data += left;
			data_len -= left;
		}
	}

	//SM3_BLOCK_SIZE 64
	while (data_len >= SM3_BLOCK_SIZE) {
		sm3_compress(ctx->digest, data);
		//块数加一
		ctx->nblocks++;
		//data指向下一块
		data += SM3_BLOCK_SIZE;
		//长度减少64
		data_len -= SM3_BLOCK_SIZE;
	}
	//最后剩下长度不满足64的部分
	ctx->num = data_len;
	//将其复制到block中
	if (data_len) {
		memcpy(ctx->block, data, data_len);
	}
}

//填充之后计算最后一块
void sm3_final(sm3_ctx_t *ctx, unsigned char *digest)
{
	int i;
	uint32_t *pdigest = (uint32_t *)digest;
	//指向补了多少个0的地方
	uint32_t *count = (uint32_t *)(ctx->block + SM3_BLOCK_SIZE - 8);
	//在block后面添加1然后补1000，0000
	ctx->block[ctx->num] = 0x80;
	//如果补了0x80和64bit（8byte)的长度后还是小于64
	if (ctx->num + 9 <= SM3_BLOCK_SIZE) {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 9);
	} else {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 1);
		sm3_compress(ctx->digest, ctx->block);
		memset(ctx->block, 0, SM3_BLOCK_SIZE - 8);
	}

	count[0] = cpu_to_be32((ctx->nblocks) >> 23);
	count[1] = cpu_to_be32((ctx->nblocks << 9) + (ctx->num << 3));

	sm3_compress(ctx->digest, ctx->block);
	for (i = 0; i < sizeof(ctx->digest)/sizeof(ctx->digest[0]); i++) {
		pdigest[i] = cpu_to_be32(ctx->digest[i]);
	}
}

#define ROTATELEFT(X,n)  (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )

void sm3_compress(uint32_t digest[8], const unsigned char block[64])
{
	int j;
	uint32_t W[68], W1[64];
	const uint32_t *pblock = (const uint32_t *)block;

	uint32_t A = digest[0];
	uint32_t B = digest[1];
	uint32_t C = digest[2];
	uint32_t D = digest[3];
	uint32_t E = digest[4];
	uint32_t F = digest[5];
	uint32_t G = digest[6];
	uint32_t H = digest[7];
	// uint32_t SS1,SS2,TT1,TT2,T[64];
	uint32_t SS1,SS2,TT1,TT2,T;

	for (j = 0; j < 16; j++) {
		W[j] = cpu_to_be32(pblock[j]);
	}
  W[16] = P1( W[0] ^ W[7] ^ ROTATELEFT(W[13],15)) ^ ROTATELEFT(W[3],7 ) ^ W[10];
  W[17] = P1( W[1] ^ W[8] ^ ROTATELEFT(W[14],15)) ^ ROTATELEFT(W[4],7 ) ^ W[11];
  W[18] = P1( W[2] ^ W[9] ^ ROTATELEFT(W[15],15)) ^ ROTATELEFT(W[5],7 ) ^ W[12];
  W[19] = P1( W[3] ^ W[10] ^ ROTATELEFT(W[16],15)) ^ ROTATELEFT(W[6],7 ) ^ W[13];
  W[20] = P1( W[4] ^ W[11] ^ ROTATELEFT(W[17],15)) ^ ROTATELEFT(W[7],7 ) ^ W[14];
  W[21] = P1( W[5] ^ W[12] ^ ROTATELEFT(W[18],15)) ^ ROTATELEFT(W[8],7 ) ^ W[15];
  W[22] = P1( W[6] ^ W[13] ^ ROTATELEFT(W[19],15)) ^ ROTATELEFT(W[9],7 ) ^ W[16];
  W[23] = P1( W[7] ^ W[14] ^ ROTATELEFT(W[20],15)) ^ ROTATELEFT(W[10],7 ) ^ W[17];
  W[24] = P1( W[8] ^ W[15] ^ ROTATELEFT(W[21],15)) ^ ROTATELEFT(W[11],7 ) ^ W[18];
  W[25] = P1( W[9] ^ W[16] ^ ROTATELEFT(W[22],15)) ^ ROTATELEFT(W[12],7 ) ^ W[19];
  W[26] = P1( W[10] ^ W[17] ^ ROTATELEFT(W[23],15)) ^ ROTATELEFT(W[13],7 ) ^ W[20];
  W[27] = P1( W[11] ^ W[18] ^ ROTATELEFT(W[24],15)) ^ ROTATELEFT(W[14],7 ) ^ W[21];
  W[28] = P1( W[12] ^ W[19] ^ ROTATELEFT(W[25],15)) ^ ROTATELEFT(W[15],7 ) ^ W[22];
  W[29] = P1( W[13] ^ W[20] ^ ROTATELEFT(W[26],15)) ^ ROTATELEFT(W[16],7 ) ^ W[23];
  W[30] = P1( W[14] ^ W[21] ^ ROTATELEFT(W[27],15)) ^ ROTATELEFT(W[17],7 ) ^ W[24];
  W[31] = P1( W[15] ^ W[22] ^ ROTATELEFT(W[28],15)) ^ ROTATELEFT(W[18],7 ) ^ W[25];
  W[32] = P1( W[16] ^ W[23] ^ ROTATELEFT(W[29],15)) ^ ROTATELEFT(W[19],7 ) ^ W[26];
  W[33] = P1( W[17] ^ W[24] ^ ROTATELEFT(W[30],15)) ^ ROTATELEFT(W[20],7 ) ^ W[27];
  W[34] = P1( W[18] ^ W[25] ^ ROTATELEFT(W[31],15)) ^ ROTATELEFT(W[21],7 ) ^ W[28];
  W[35] = P1( W[19] ^ W[26] ^ ROTATELEFT(W[32],15)) ^ ROTATELEFT(W[22],7 ) ^ W[29];
  W[36] = P1( W[20] ^ W[27] ^ ROTATELEFT(W[33],15)) ^ ROTATELEFT(W[23],7 ) ^ W[30];
  W[37] = P1( W[21] ^ W[28] ^ ROTATELEFT(W[34],15)) ^ ROTATELEFT(W[24],7 ) ^ W[31];
  W[38] = P1( W[22] ^ W[29] ^ ROTATELEFT(W[35],15)) ^ ROTATELEFT(W[25],7 ) ^ W[32];
  W[39] = P1( W[23] ^ W[30] ^ ROTATELEFT(W[36],15)) ^ ROTATELEFT(W[26],7 ) ^ W[33];
  W[40] = P1( W[24] ^ W[31] ^ ROTATELEFT(W[37],15)) ^ ROTATELEFT(W[27],7 ) ^ W[34];
  W[41] = P1( W[25] ^ W[32] ^ ROTATELEFT(W[38],15)) ^ ROTATELEFT(W[28],7 ) ^ W[35];
  W[42] = P1( W[26] ^ W[33] ^ ROTATELEFT(W[39],15)) ^ ROTATELEFT(W[29],7 ) ^ W[36];
  W[43] = P1( W[27] ^ W[34] ^ ROTATELEFT(W[40],15)) ^ ROTATELEFT(W[30],7 ) ^ W[37];
  W[44] = P1( W[28] ^ W[35] ^ ROTATELEFT(W[41],15)) ^ ROTATELEFT(W[31],7 ) ^ W[38];
  W[45] = P1( W[29] ^ W[36] ^ ROTATELEFT(W[42],15)) ^ ROTATELEFT(W[32],7 ) ^ W[39];
  W[46] = P1( W[30] ^ W[37] ^ ROTATELEFT(W[43],15)) ^ ROTATELEFT(W[33],7 ) ^ W[40];
  W[47] = P1( W[31] ^ W[38] ^ ROTATELEFT(W[44],15)) ^ ROTATELEFT(W[34],7 ) ^ W[41];
  W[48] = P1( W[32] ^ W[39] ^ ROTATELEFT(W[45],15)) ^ ROTATELEFT(W[35],7 ) ^ W[42];
  W[49] = P1( W[33] ^ W[40] ^ ROTATELEFT(W[46],15)) ^ ROTATELEFT(W[36],7 ) ^ W[43];
  W[50] = P1( W[34] ^ W[41] ^ ROTATELEFT(W[47],15)) ^ ROTATELEFT(W[37],7 ) ^ W[44];
  W[51] = P1( W[35] ^ W[42] ^ ROTATELEFT(W[48],15)) ^ ROTATELEFT(W[38],7 ) ^ W[45];
  W[52] = P1( W[36] ^ W[43] ^ ROTATELEFT(W[49],15)) ^ ROTATELEFT(W[39],7 ) ^ W[46];
  W[53] = P1( W[37] ^ W[44] ^ ROTATELEFT(W[50],15)) ^ ROTATELEFT(W[40],7 ) ^ W[47];
  W[54] = P1( W[38] ^ W[45] ^ ROTATELEFT(W[51],15)) ^ ROTATELEFT(W[41],7 ) ^ W[48];
  W[55] = P1( W[39] ^ W[46] ^ ROTATELEFT(W[52],15)) ^ ROTATELEFT(W[42],7 ) ^ W[49];
  W[56] = P1( W[40] ^ W[47] ^ ROTATELEFT(W[53],15)) ^ ROTATELEFT(W[43],7 ) ^ W[50];
  W[57] = P1( W[41] ^ W[48] ^ ROTATELEFT(W[54],15)) ^ ROTATELEFT(W[44],7 ) ^ W[51];
  W[58] = P1( W[42] ^ W[49] ^ ROTATELEFT(W[55],15)) ^ ROTATELEFT(W[45],7 ) ^ W[52];
  W[59] = P1( W[43] ^ W[50] ^ ROTATELEFT(W[56],15)) ^ ROTATELEFT(W[46],7 ) ^ W[53];
  W[60] = P1( W[44] ^ W[51] ^ ROTATELEFT(W[57],15)) ^ ROTATELEFT(W[47],7 ) ^ W[54];
  W[61] = P1( W[45] ^ W[52] ^ ROTATELEFT(W[58],15)) ^ ROTATELEFT(W[48],7 ) ^ W[55];
  W[62] = P1( W[46] ^ W[53] ^ ROTATELEFT(W[59],15)) ^ ROTATELEFT(W[49],7 ) ^ W[56];
  W[63] = P1( W[47] ^ W[54] ^ ROTATELEFT(W[60],15)) ^ ROTATELEFT(W[50],7 ) ^ W[57];
  W[64] = P1( W[48] ^ W[55] ^ ROTATELEFT(W[61],15)) ^ ROTATELEFT(W[51],7 ) ^ W[58];
  W[65] = P1( W[49] ^ W[56] ^ ROTATELEFT(W[62],15)) ^ ROTATELEFT(W[52],7 ) ^ W[59];
  W[66] = P1( W[50] ^ W[57] ^ ROTATELEFT(W[63],15)) ^ ROTATELEFT(W[53],7 ) ^ W[60];
  W[67] = P1( W[51] ^ W[58] ^ ROTATELEFT(W[64],15)) ^ ROTATELEFT(W[54],7 ) ^ W[61];
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,0)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[0];
  TT2 = GG0(E,F,G) + H + SS1 + W[0];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,1)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[1];
  TT2 = GG0(E,F,G) + H + SS1 + W[1];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,2)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[2];
  TT2 = GG0(E,F,G) + H + SS1 + W[2];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,3)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[3];
  TT2 = GG0(E,F,G) + H + SS1 + W[3];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,4)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[4];
  TT2 = GG0(E,F,G) + H + SS1 + W[4];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,5)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[5];
  TT2 = GG0(E,F,G) + H + SS1 + W[5];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,6)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[6];
  TT2 = GG0(E,F,G) + H + SS1 + W[6];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,7)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[7];
  TT2 = GG0(E,F,G) + H + SS1 + W[7];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,8)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[8];
  TT2 = GG0(E,F,G) + H + SS1 + W[8];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,9)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[9];
  TT2 = GG0(E,F,G) + H + SS1 + W[9];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,10)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[10];
  TT2 = GG0(E,F,G) + H + SS1 + W[10];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,11)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[11];
  TT2 = GG0(E,F,G) + H + SS1 + W[11];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,12)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[12];
  TT2 = GG0(E,F,G) + H + SS1 + W[12];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,13)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[13];
  TT2 = GG0(E,F,G) + H + SS1 + W[13];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,14)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[14];
  TT2 = GG0(E,F,G) + H + SS1 + W[14];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x79CC4519;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,15)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF0(A,B,C) + D + SS2 + W1[15];
  TT2 = GG0(E,F,G) + H + SS1 + W[15];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);
  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,16)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[16];
  TT2 = GG1(E,F,G) + H + SS1 + W[16];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,17)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[17];
  TT2 = GG1(E,F,G) + H + SS1 + W[17];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,18)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[18];
  TT2 = GG1(E,F,G) + H + SS1 + W[18];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,19)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[19];
  TT2 = GG1(E,F,G) + H + SS1 + W[19];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,20)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[20];
  TT2 = GG1(E,F,G) + H + SS1 + W[20];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,21)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[21];
  TT2 = GG1(E,F,G) + H + SS1 + W[21];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,22)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[22];
  TT2 = GG1(E,F,G) + H + SS1 + W[22];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,23)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[23];
  TT2 = GG1(E,F,G) + H + SS1 + W[23];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,24)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[24];
  TT2 = GG1(E,F,G) + H + SS1 + W[24];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,25)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[25];
  TT2 = GG1(E,F,G) + H + SS1 + W[25];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,26)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[26];
  TT2 = GG1(E,F,G) + H + SS1 + W[26];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,27)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[27];
  TT2 = GG1(E,F,G) + H + SS1 + W[27];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,28)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[28];
  TT2 = GG1(E,F,G) + H + SS1 + W[28];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,29)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[29];
  TT2 = GG1(E,F,G) + H + SS1 + W[29];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,30)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[30];
  TT2 = GG1(E,F,G) + H + SS1 + W[30];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,31)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[31];
  TT2 = GG1(E,F,G) + H + SS1 + W[31];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,32)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[32];
  TT2 = GG1(E,F,G) + H + SS1 + W[32];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,33)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[33];
  TT2 = GG1(E,F,G) + H + SS1 + W[33];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,34)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[34];
  TT2 = GG1(E,F,G) + H + SS1 + W[34];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,35)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[35];
  TT2 = GG1(E,F,G) + H + SS1 + W[35];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,36)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[36];
  TT2 = GG1(E,F,G) + H + SS1 + W[36];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,37)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[37];
  TT2 = GG1(E,F,G) + H + SS1 + W[37];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,38)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[38];
  TT2 = GG1(E,F,G) + H + SS1 + W[38];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,39)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[39];
  TT2 = GG1(E,F,G) + H + SS1 + W[39];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,40)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[40];
  TT2 = GG1(E,F,G) + H + SS1 + W[40];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,41)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[41];
  TT2 = GG1(E,F,G) + H + SS1 + W[41];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,42)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[42];
  TT2 = GG1(E,F,G) + H + SS1 + W[42];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,43)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[43];
  TT2 = GG1(E,F,G) + H + SS1 + W[43];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,44)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[44];
  TT2 = GG1(E,F,G) + H + SS1 + W[44];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,45)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[45];
  TT2 = GG1(E,F,G) + H + SS1 + W[45];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,46)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[46];
  TT2 = GG1(E,F,G) + H + SS1 + W[46];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,47)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[47];
  TT2 = GG1(E,F,G) + H + SS1 + W[47];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,48)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[48];
  TT2 = GG1(E,F,G) + H + SS1 + W[48];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,49)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[49];
  TT2 = GG1(E,F,G) + H + SS1 + W[49];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,50)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[50];
  TT2 = GG1(E,F,G) + H + SS1 + W[50];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,51)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[51];
  TT2 = GG1(E,F,G) + H + SS1 + W[51];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,52)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[52];
  TT2 = GG1(E,F,G) + H + SS1 + W[52];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,53)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[53];
  TT2 = GG1(E,F,G) + H + SS1 + W[53];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,54)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[54];
  TT2 = GG1(E,F,G) + H + SS1 + W[54];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,55)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[55];
  TT2 = GG1(E,F,G) + H + SS1 + W[55];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,56)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[56];
  TT2 = GG1(E,F,G) + H + SS1 + W[56];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,57)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[57];
  TT2 = GG1(E,F,G) + H + SS1 + W[57];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,58)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[58];
  TT2 = GG1(E,F,G) + H + SS1 + W[58];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,59)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[59];
  TT2 = GG1(E,F,G) + H + SS1 + W[59];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,60)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[60];
  TT2 = GG1(E,F,G) + H + SS1 + W[60];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,61)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[61];
  TT2 = GG1(E,F,G) + H + SS1 + W[61];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,62)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[62];
  TT2 = GG1(E,F,G) + H + SS1 + W[62];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

  T = 0x7A879D8A;
  SS1 = ROTATELEFT((ROTATELEFT(A,12) + E + ROTATELEFT(T,63)), 7);
  SS2 = SS1 ^ ROTATELEFT(A,12);
  TT1 = FF1(A,B,C) + D + SS2 + W1[63];
  TT2 = GG1(E,F,G) + H + SS1 + W[63];
  D = C;
  C = ROTATELEFT(B,9);
  B = A;
  A = TT1;
  H = G;
  G = ROTATELEFT(F,19);
  F = E;
  E = P0(TT2);

	digest[0] ^= A;
	digest[1] ^= B;
	digest[2] ^= C;
	digest[3] ^= D;
	digest[4] ^= E;
	digest[5] ^= F;
	digest[6] ^= G;
	digest[7] ^= H;
}

void sm3(const unsigned char *msg, size_t msglen,
	unsigned char dgst[SM3_DIGEST_LENGTH])
{
	sm3_ctx_t ctx;

	sm3_init(&ctx);
	sm3_update(&ctx, msg, msglen);
	sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(sm3_ctx_t));
}
