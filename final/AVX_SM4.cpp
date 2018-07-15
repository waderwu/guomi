#include <stdlib.h>
// #include "romangol.h"
// #include "liarod.h"
#include <time.h>
#include "AVX_SM4.h"
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
/*
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
*/
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
inline void sms4_avx2_encrypt_8blocks(const unsigned char *in, int *out, const u4 *key)
{
	GET_BLKS(x0, x1, x2, x3, in);

	ROUNDS(x0, x1, x2, x3, x4);
	t0 = _mm256_shuffle_epi8(x0, vindex_swap);
	t1 = _mm256_shuffle_epi8(x4, vindex_swap);
	t2 = _mm256_shuffle_epi8(x3, vindex_swap);
	t3 = _mm256_shuffle_epi8(x2, vindex_swap);
	forloop(ii, 0, 4) {
		x0 = t0;
		x1 = _mm256_slli_si256(t1, 4);
		x2 = _mm256_slli_si256(t2, 8);
		x3 = _mm256_slli_si256(t3, 12);
		mask = _mm256_setr_epi32(4294967295, 0, 0, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x0);
		mask = _mm256_setr_epi32(0, 4294967295, 0, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x1);
		mask = _mm256_setr_epi32(0, 0, 4294967295, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x2);
		mask = _mm256_setr_epi32(0, 0, 0, 4294967295, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x3);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 4294967295, 0, 0, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x0);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 4294967295, 0, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x1);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 4294967295, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x2);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 4294967295);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x3);
		t0 = _mm256_srli_si256(t0, 4);
		t1 = _mm256_srli_si256(t1, 4);
		t2 = _mm256_srli_si256(t2, 4);
		t3 = _mm256_srli_si256(t3, 4);
	}
}
inline void sms4_avx2_decrypt_8blocks(const unsigned char *in, int *out, const u4 *key)
{
	GET_BLKS(x0, x1, x2, x3, in);

	ROUNDS_DEC(x0, x1, x2, x3, x4);
	t0 = _mm256_shuffle_epi8(x0, vindex_swap);
	t1 = _mm256_shuffle_epi8(x4, vindex_swap);
	t2 = _mm256_shuffle_epi8(x3, vindex_swap);
	t3 = _mm256_shuffle_epi8(x2, vindex_swap);
	forloop(ii, 0, 4) {
		x0 = t0;
		x1 = _mm256_slli_si256(t1, 4);
		x2 = _mm256_slli_si256(t2, 8);
		x3 = _mm256_slli_si256(t3, 12);
		mask = _mm256_setr_epi32(4294967295, 0, 0, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x0);
		mask = _mm256_setr_epi32(0, 4294967295, 0, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x1);
		mask = _mm256_setr_epi32(0, 0, 4294967295, 0, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x2);
		mask = _mm256_setr_epi32(0, 0, 0, 4294967295, 0, 0, 0, 0);
		_mm256_maskstore_epi32(out + ii * 4, mask, x3);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 4294967295, 0, 0, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x0);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 4294967295, 0, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x1);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 4294967295, 0);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x2);
		mask = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 4294967295);
		_mm256_maskstore_epi32(out + 12 + ii * 4, mask, x3);
		t0 = _mm256_srli_si256(t0, 4);
		t1 = _mm256_srli_si256(t1, 4);
		t2 = _mm256_srli_si256(t2, 4);
		t3 = _mm256_srli_si256(t3, 4);
	}
}
void sms4_avx2_encrypt_16blocks(const unsigned char *in, int *out, const u4 *key)
{
	sms4_avx2_encrypt_8blocks(in,out,key);
	sms4_avx2_encrypt_8blocks(in + SM4_BLOCK_SIZE * 8, out + SM4_BLOCK_SIZE *8/4, key);
}
void sms4_avx2_decrypt_16blocks(const unsigned char *in, int *out, const u4 *key)
{
	sms4_avx2_decrypt_8blocks(in,out,key);
	sms4_avx2_decrypt_8blocks(in + SM4_BLOCK_SIZE * 8, out + SM4_BLOCK_SIZE *8/4, key);
}
inline void sms4_init_sbox32(void)
{
	int i, j;
	uint32_t a;
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			a = Sbox[i] << 8 | Sbox[j];
			SBOX32L[(i << 8) + j] = a;
			SBOX32H[(i << 8) + j] = a << 16;
		}
	}
}
void sms4_avx2_encrypt_init(const u4 *key)
{
	mask_ffff = _mm256_set1_epi32(0xffff);
	vindex_0s = _mm256_set1_epi32(0);
	vindex_4i = _mm256_setr_epi32(0,4,8,12,16,20,24,28);
	vindex_read = _mm256_setr_epi32(0,8,16,24,1,9,17,25);
	vindex_swap = _mm256_setr_epi8(
		3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
		3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12
	);
	sms4_init_sbox32();
}
