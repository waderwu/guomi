#include "sm4_share.h"


void test(u1 *p, u1*c, u4 *rkey, int blockSize)
{
  clock_t t=clock();

	// speed_test_share<<<1,1>>>(1,p, c, rkey );
  // int blockSize = 512;
  int numBlocks = (test_blocks + blockSize -1) /blockSize;
  speed_test_share<<<numBlocks,blockSize>>>(test_blocks,p,c,rkey);

	cudaDeviceSynchronize();
  double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (16*test_blocks)/(1024*1024*tt);

	printf("thrads: %d \ntime: %f s\nspeed: %f \n",blockSize,tt,speed);
}

int main()
{
  InitCUDA();
	u1 * key;
	cudaMallocManaged(&key,sizeof(u1)*16);
	u4 * rkey;
	cudaMallocManaged(&rkey,sizeof(u4)*(SM4_RND_KEY_SIZE / sizeof(u4)));
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



	SM4_key_schedule<<<1,1>>>( key, rkey );
	cudaDeviceSynchronize();

  test(p,c,rkey,32);
  test(p,c,rkey,64);
  test(p,c,rkey,128);
  test(p,c,rkey,256);
  test(p,c,rkey,512);
  test(p,c,rkey,1024);
  // test(p,c,rkey,test_blocks);
	// printf("hahahaha\n");

	// for (int j=0 ; j<32; j++)
	// {
	// 	printf("%04x,",rkey[j]);
	// }



	//for (int k=0; k<16*test_blocks; k++)
	//{
	//	printf("%02x ",c[k]);
	//}
}
