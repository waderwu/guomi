#include "GPU_SM4.h"

#define test_blocks (8)

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

  	gpu_sm4_encrypt(p,key,c,test_blocks);

  	outputChar(c, 16);

  	gpu_sm4_decrypt(p,key,c,test_blocks);

  	outputChar(p, 16);

}
