#include "sm3fast.h"
#define ROTATELEFT(X,n)  (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )


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



// void OneRound(int i,uint32_t *A, uint32_t *B, uint32_t *C, uint32_t *D, uint32_t *E, uint32_t *F, uint32_t *G , uint32_t *H, uint32_t *W){
//   uint32_t SS1,SS2,TT1,TT2,T;
//   uint32_t t[64] ={
//     0x79cc4519,0xf3988a32,0x1e7311465,0x3ce6228cb,0x79cc45197,0xf3988a32f,0x1e7311465e,
//     0x3ce6228cbc,0x79cc451979,0xf3988a32f3,0x1e7311465e7,0x3ce6228cbce,0x79cc451979c,
//     0xf3988a32f39,0x1e7311465e73,0x3ce6228cbce6,0x7a879d8a7a87,0xf50f3b14f50f,0x1ea1e7629ea1e,
//     0x3d43cec53d43c,0x7a879d8a7a879,0xf50f3b14f50f3,0x1ea1e7629ea1e7,0x3d43cec53d43ce,
//     0x7a879d8a7a879d,0xf50f3b14f50f3b,0x1ea1e7629ea1e76,0x3d43cec53d43cec,0x7a879d8a7a879d8,
//     0xf50f3b14f50f3b1,0x1ea1e7629ea1e762,0x3d43cec53d43cec5,0x7a879d8a,0xf50f3b14,0x1ea1e7629,
//     0x3d43cec53,0x7a879d8a7,0xf50f3b14f,0x1ea1e7629e,0x3d43cec53d,0x7a879d8a7a,0xf50f3b14f5,
//     0x1ea1e7629ea,0x3d43cec53d4,0x7a879d8a7a8,0xf50f3b14f50,0x1ea1e7629ea1,0x3d43cec53d43,0x7a879d8a7a87,
//     0xf50f3b14f50f,0x1ea1e7629ea1e,0x3d43cec53d43c,0x7a879d8a7a879,0xf50f3b14f50f3,0x1ea1e7629ea1e7,0x3d43cec53d43ce,
//     0x7a879d8a7a879d,0xf50f3b14f50f3b,0x1ea1e7629ea1e76,0x3d43cec53d43cec,0x7a879d8a7a879d8,0xf50f3b14f50f3b1,
//     0x1ea1e7629ea1e762,0x3d43cec53d43cec5};
//
//   if (i>=12){
//     W[i+4] = P1( W[i-12] ^ W[i-5] ^ ROTATELEFT(W[i+1],15)) ^ ROTATELEFT(W[i-9],7 ) ^ W[i-2];
//   }
//
//   if (i <= 15){
//     TT2 = ROTATELEFT(*A,12);
//     TT1 = TT2 + *E + t[i];
//     TT1 = ROTATELEFT(TT1,7);
//     TT2 ^= TT1;
//     *D = *D + FF0(*A,*B,*C) + TT2 + (W[i] ^ W[i+4]);
//     *H = *H + GG0(*E,*F,*G) + TT1 + W[i];
//     *B = ROTATELEFT(*B, 9);
//     *F = ROTATELEFT(*F ,19);
//     *H = P0(*H);
//   }else{
//     TT2 = ROTATELEFT(*A,12);
//     TT1 = TT2 + *E + t[i];
//     TT1 = ROTATELEFT(TT1,7);
//     TT2 ^= TT1;
//     *D = *D + FF1(*A,*B,*C) + TT2 + (W[i] ^ W[i+4]);
//     *H = *H + GG1(*E,*F,*G) + TT1 + W[i];
//     *B = ROTATELEFT(*B,9);
//     *F = ROTATELEFT(*F ,19);
//     *H = P0(*H);
//   }
// }



void sm3(const unsigned char *msg, size_t msglen,
	unsigned char dgst[SM3_DIGEST_LENGTH])
{
	sm3_ctx_t ctx;

	sm3_init(&ctx);
	sm3_update(&ctx, msg, msglen);
	sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(sm3_ctx_t));
}
