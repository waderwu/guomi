#include "GPU_SM4.h"

#define test_blocks (1<<25)

int main()
{
	u1 * key;
	cudaMallocManaged(&key,sizeof(u1)*16);
	u1 * p;
	cudaMallocManaged(&p,sizeof(u1)*16*test_blocks);

	u1 *c;
	cudaMallocManaged(&c,sizeof(u1)*16*test_blocks);

	u1 p2[SM4_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

	u1 key2[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	for (int i=0; i<16; i++)
	{
		key[i] = key2[i];
	}

  for (int i=0; i<test_blocks*16; i++)
  {
    p[i] = p2[i%16];
  }

  gpu_sm4_encrypt(p,key,c,4);

  outputChar(c, 16*4);

  gpu_sm4_decrypt(p,key,c,4);

  outputChar(p, 16*4);

	//benchmark
	benchmark_sm4_encrypt(p,key,c,1<<4);
	benchmark_sm4_decrypt(p,key,c,1<<4);

	benchmark_sm4_encrypt(p,key,c,1<<8);
	benchmark_sm4_decrypt(p,key,c,1<<8);

	benchmark_sm4_encrypt(p,key,c,1<<10);
	benchmark_sm4_decrypt(p,key,c,1<<10);

	benchmark_sm4_encrypt(p,key,c,1<<12);
	benchmark_sm4_decrypt(p,key,c,1<<12);

	benchmark_sm4_encrypt(p,key,c,1<<16);
	benchmark_sm4_decrypt(p,key,c,1<<16);

	benchmark_sm4_encrypt(p,key,c,1<<18);
	benchmark_sm4_decrypt(p,key,c,1<<18);

	benchmark_sm4_encrypt(p,key,c,1<<20);
	benchmark_sm4_decrypt(p,key,c,1<<20);

	benchmark_sm4_encrypt(p,key,c,test_blocks);
	benchmark_sm4_decrypt(p,key,c,test_blocks);
}
