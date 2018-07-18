#include "sm3.h"

#define test_len (1<<20)

void test(char *msg, int length, char *dgst)
{
	clock_t t=clock();
	for (int i=0; i<100; i++)
	{
		sm3(msg,length,dgst);
	}

  double tt = (double)(clock() - t)/(CLOCKS_PER_SEC*100);
  double speed =(double) (length)/(1024*1024*tt);
	printf("size: %d time: %f s speed: %f MB/s\n",length,tt,speed);
}

int main()
{
	char *msg = (char *)malloc(sizeof(char)*test_len);
  for (int i=0; i < test_len; i++)
  {
    msg[i] = 'a';
  }



	char dgst[SM3_DIGEST_LENGTH];
	test(msg,16,dgst);
	test(msg,64,dgst);
	test(msg,256,dgst);
	test(msg,1024,dgst);
	test(msg,8192,dgst);
	test(msg,16384,dgst);
	test(msg,test_len,dgst);

	for (int j=0 ; j<SM3_DIGEST_LENGTH; j++ )
	{
		printf("%hhX,",dgst[j]);
	}
}
