#ifndef SPH_SM3_HASH_4WAY_H
#define SPH_SM3_HASH_4WAY_H

#define SM3_DIGEST_LENGTH	32
#define SM3_BLOCK_SIZE		64
#define SM3_CBLOCK		(SM3_BLOCK_SIZE)
#define SM3_HMAC_SIZE		(SM3_DIGEST_LENGTH)


#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include "avxdef.h"

#ifdef __cplusplus
extern "C" {
#endif


	typedef struct {
		__m128i block[16];
		__m128i digest[8];
		uint32_t nblocks;
		uint32_t num;
	} sm3_4way_ctx_t;

	void sm3_4way_init(sm3_4way_ctx_t *ctx);
	void sm3_4way_compress(__m128i *digest, __m128i *block);
	void sm3_4way(void *cc, const void *data, size_t len);
	void sm3_4way_close(void *cc, void *dst);
	void proc_output(char* output1, char* output2, char* output3, char* output4, __m128i* output);
	void proc_input(char* input1, char* input2, char* input3, char* input4, __m128i* input, int maxlen);
	void AVX_SM3(char* input1, char* input2, char* input3, char* input4, char* output1, char* output2, char* output3, char* output4);

#ifdef __cplusplus
}
#endif
#endif