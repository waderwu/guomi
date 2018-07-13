#ifndef HEADER_SM3FAST_H
#define HEADER_SM3FAST_H

#ifndef OPENSSL_NO_SM3

#define SM3_DIGEST_LENGTH	32
#define SM3_BLOCK_SIZE		64
#define SM3_CBLOCK		(SM3_BLOCK_SIZE)
#define SM3_HMAC_SIZE		(SM3_DIGEST_LENGTH)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned int uint32_t;

#include <string.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif



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

typedef struct {
	uint32_t digest[8];
	int nblocks;
	unsigned char block[64];
	int num;
} sm3_ctx_t;

void sm3_init(sm3_ctx_t *ctx);
void sm3_update(sm3_ctx_t *ctx, const unsigned char* data, size_t data_len);
void sm3_final(sm3_ctx_t *ctx, unsigned char digest[SM3_DIGEST_LENGTH]);
void sm3_compress(uint32_t digest[8], const unsigned char block[SM3_BLOCK_SIZE]);
void sm3(const unsigned char *data, size_t datalen,
	unsigned char digest[SM3_DIGEST_LENGTH]);


typedef struct {
	sm3_ctx_t sm3_ctx;
	unsigned char key[SM3_BLOCK_SIZE];
} sm3_hmac_ctx_t;

void sm3_hmac_init(sm3_hmac_ctx_t *ctx, const unsigned char *key, size_t key_len);
void sm3_hmac_update(sm3_hmac_ctx_t *ctx, const unsigned char *data, size_t data_len);
void sm3_hmac_final(sm3_hmac_ctx_t *ctx, unsigned char mac[SM3_HMAC_SIZE]);
void sm3_hmac(const unsigned char *data, size_t data_len,
	const unsigned char *key, size_t key_len, unsigned char mac[SM3_HMAC_SIZE]);

#ifdef __cplusplus
}
#endif
#endif
#endif
