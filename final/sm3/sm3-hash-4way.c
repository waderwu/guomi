#include <string.h>
#include <stdio.h>
#include "sm3-hash-4way.h"


#define P0(x) _mm_xor_si128( x, _mm_xor_si128( mm_rol_32( x,  9 ), \
                                               mm_rol_32( x, 17 ) ) ) 
#define P1(x) _mm_xor_si128( x, _mm_xor_si128( mm_rol_32( x, 15 ), \
                                               mm_rol_32( x, 23 ) ) ) 

#define FF0(x,y,z) _mm_xor_si128( x, _mm_xor_si128( y, z ) )
#define FF1(x,y,z) _mm_or_si128( _mm_or_si128( _mm_and_si128( x, y ), \
                                               _mm_and_si128( x, z ) ), \
                                               _mm_and_si128( y, z ) )

#define GG0(x,y,z) FF0(x,y,z)
#define GG1(x,y,z) _mm_or_si128( _mm_and_si128( x, y ), \
                                 _mm_andnot_si128( x, z ) )

void sm3_4way_init(sm3_4way_ctx_t *ctx);
void sm3_4way(void *cc, const void *data, size_t len);
void sm3_4way_close(void *cc, void *dst);
void sm3_4way_compress(__m128i *digest, __m128i *block);

void sm3_4way_init(sm3_4way_ctx_t *ctx)
{
	ctx->digest[0] = _mm_set1_epi32(0x7380166F);
	ctx->digest[1] = _mm_set1_epi32(0x4914B2B9);
	ctx->digest[2] = _mm_set1_epi32(0x172442D7);
	ctx->digest[3] = _mm_set1_epi32(0xDA8A0600);
	ctx->digest[4] = _mm_set1_epi32(0xA96F30BC);
	ctx->digest[5] = _mm_set1_epi32(0x163138AA);
	ctx->digest[6] = _mm_set1_epi32(0xE38DEE4D);
	ctx->digest[7] = _mm_set1_epi32(0xB0FB0E4E);
	ctx->nblocks = 0;
	ctx->num = 0;
}

void sm3_4way(void *cc, const void *data, size_t len)
{
	sm3_4way_ctx_t *ctx = (sm3_4way_ctx_t*)cc;
	__m128i *block = (__m128i*)ctx->block;
	__m128i *vdata = (__m128i*)data;

	if (ctx->num)
	{
		unsigned int left = SM3_BLOCK_SIZE - ctx->num;
		if (len < left)
		{
			memcpy_128(block + (ctx->num >> 2), vdata, len >> 2);
			ctx->num += len;
			return;
		}
		else
		{
			memcpy_128(block + (ctx->num >> 2), vdata, left >> 2);
			sm3_4way_compress(ctx->digest, block);
			ctx->nblocks++;
			vdata += left >> 2;
			len -= left;
		}
	}
	while (len >= SM3_BLOCK_SIZE)
	{
		sm3_4way_compress(ctx->digest, vdata);
		ctx->nblocks++;
		vdata += SM3_BLOCK_SIZE >> 2;
		len -= SM3_BLOCK_SIZE;
	}
	ctx->num = len;
	if (len)
		memcpy_128(block, vdata, len/4+1);
}

void sm3_4way_close(void *cc, void *dst)
{
	sm3_4way_ctx_t *ctx = (sm3_4way_ctx_t*)cc;
	__m128i *hash = (__m128i*)dst;
	__m128i *count = (__m128i*)(ctx->block + ((SM3_BLOCK_SIZE - 8) >> 2));
	__m128i *block = (__m128i*)ctx->block;
	int i;

	//block[ctx->num] = _mm_set1_epi32(0x80);

	if (ctx->num + 8 <= SM3_BLOCK_SIZE)
	{
		memset_zero_128(block + (ctx->num >> 2) + 1,
			(SM3_BLOCK_SIZE - ctx->num - 8) >> 2);
	}
	else
	{
		memset_zero_128(block + (ctx->num >> 2) + 1,
			(SM3_BLOCK_SIZE - (ctx->num >> 2) - 1));
		sm3_4way_compress(ctx->digest, block);
		memset_zero_128(block, (SM3_BLOCK_SIZE - 8) >> 2);
	}

	count[0] = mm_bswap_32(
		_mm_set1_epi32(ctx->nblocks >> 23));
	count[1] = mm_bswap_32(_mm_set1_epi32((ctx->nblocks << 9) +
		(ctx->num << 3)));
	sm3_4way_compress(ctx->digest, block);

	for (i = 0; i < 8; i++)
		hash[i] = mm_bswap_32(ctx->digest[i]);
}

void sm3_4way_compress(__m128i *digest, __m128i *block)
{
	__m128i W[68], W1[64];
	__m128i A = digest[0];
	__m128i B = digest[1];
	__m128i C = digest[2];
	__m128i D = digest[3];
	__m128i E = digest[4];
	__m128i F = digest[5];
	__m128i G = digest[6];
	__m128i H = digest[7];
	__m128i SS1, SS2, TT1, TT2, T;
	int j;

	for (j = 0; j < 16; j++)
		W[j] = mm_bswap_32(block[j]);

	for (j = 16; j < 68; j++)
		W[j] = _mm_xor_si128(P1(_mm_xor_si128(_mm_xor_si128(W[j - 16],
			W[j - 9]),
			mm_rol_32(W[j - 3], 15))),
			_mm_xor_si128(mm_rol_32(W[j - 13], 7),
				W[j - 6]));

	for (j = 0; j < 64; j++)
		W1[j] = _mm_xor_si128(W[j], W[j + 4]);

	T = _mm_set1_epi32(0x79CC4519UL);
	for (j = 0; j < 16; j++)
	{
		SS1 = mm_rol_32(_mm_add_epi32(_mm_add_epi32(mm_rol_32(A, 12), E),
			mm_rol_32(T, j)), 7);
		SS2 = _mm_xor_si128(SS1, mm_rol_32(A, 12));
		TT1 = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(FF0(A, B, C), D),
			SS2), W1[j]);
		TT2 = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(GG0(E, F, G), H),
			SS1), W[j]);
		D = C;
		C = mm_rol_32(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = mm_rol_32(F, 19);
		F = E;
		E = P0(TT2);
	}

	T = _mm_set1_epi32(0x7A879D8AUL);
	for (j = 16; j < 64; j++)
	{
		SS1 = mm_rol_32(_mm_add_epi32(_mm_add_epi32(mm_rol_32(A, 12), E),
			mm_rol_32(T, j & 31)), 7);
		SS2 = _mm_xor_si128(SS1, mm_rol_32(A, 12));
		TT1 = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(FF1(A, B, C), D),
			SS2), W1[j]);
		TT2 = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(GG1(E, F, G), H),
			SS1), W[j]);
		D = C;
		C = mm_rol_32(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = mm_rol_32(F, 19);
		F = E;
		E = P0(TT2);
	}

	digest[0] = _mm_xor_si128(digest[0], A);
	digest[1] = _mm_xor_si128(digest[1], B);
	digest[2] = _mm_xor_si128(digest[2], C);
	digest[3] = _mm_xor_si128(digest[3], D);
	digest[4] = _mm_xor_si128(digest[4], E);
	digest[5] = _mm_xor_si128(digest[5], F);
	digest[6] = _mm_xor_si128(digest[6], G);
	digest[7] = _mm_xor_si128(digest[7], H);
}



void proc_input(char* input1, char* input2, char* input3, char* input4, __m128i* input, int maxlen) {
	char buff[0x10];
	__m128i vindex = _mm_setr_epi32(0, 1, 2, 3);
	strcat(input1, "\x80");
	strcat(input2, "\x80");
	strcat(input3, "\x80");
	strcat(input4, "\x80");
	for (int i = 0; i < (maxlen - 1) / 4 + 1; i++) {
		memcpy(&buff[0], input1, 4);
		memcpy(&buff[4], input2, 4);
		memcpy(&buff[8], input3, 4);
		memcpy(&buff[12], input4, 4);
		input[i] = _mm_i32gather_epi32((int*)buff, vindex, 4);
	}
}
void proc_output(char* output1, char* output2, char* output3, char* output4, __m128i* output) {
	char buffer[0x20];
	for (int i = 0; i < 16; i++) {
		_mm_store_si128((__m128i*)buffer, output[i]);
		memcpy(&output1[i * 4], &buffer[0], 4);
		memcpy(&output2[i * 4], &buffer[4], 4);
		memcpy(&output3[i * 4], &buffer[8], 4);
		memcpy(&output4[i * 4], &buffer[12], 4);
	}
}
inline int max(int x1, int x2) {
	return (x1 > x2) ? x1 : x2;
}
__m128i input[2<<18];
__m128i output[8];
void AVX_SM3(char* input1, char* input2, char* input3, char* input4, char* output1, char* output2, char* output3, char* output4) {

	sm3_4way_ctx_t ctx;
	int maxlen = max(max(max(strlen(input1), strlen(input2)), strlen(input3)), strlen(input4));
	proc_input(input1, input2, input3, input4, input, maxlen);
	sm3_4way_init(&ctx);
	sm3_4way(&ctx, input, maxlen);
	sm3_4way_close(&ctx, output);
	proc_output(output1, output2, output3, output4, output);
}