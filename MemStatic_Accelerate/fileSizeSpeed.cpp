#include "romangol.h"
#include "liarod.h"
#include "sm4fast.h"
#include <time.h>

void test(u1 *p, u1*c, u4 *rkey, int blocks)
{
	clock_t t = clock();
	for (int j=0 ; j< blocks; j++)
	{
		SM4_enc_block(p + 16*j, c + 16 *j, rkey);
	}
	double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (blocks*16)/(1024*1024*tt);
	printf("size:%d kB\n time: %f s\nspeed: %f MB/s\n",16*blocks, tt,speed);
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


	u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)];
	SM4_key_schedule( key, rkey );
	// outputDword(rkey, 32);
  printf("%s\n", "MemStatic file size test");
	test(p,c,rkey,1);
  test(p,c,rkey,4);
  test(p,c,rkey,16);
  test(p,c,rkey,64);
  test(p,c,rkey,512);
  test(p,c,rkey,1024);
	test(p,c,rkey,test_blocks);
}
