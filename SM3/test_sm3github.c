#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sm3github.h"

#define SM3_DIGEST_LENGTH 32

#define test_len (16384)

int main()
{
	char *msg = (char *)malloc(sizeof(char)*test_len);
  for (int i=0; i < test_len; i++)
  {
    msg[i] = 'a';
  }



	char dgst[SM3_DIGEST_LENGTH];
  clock_t t=clock();
	sm3(msg,test_len,dgst);
  double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (test_len)/(1024*1024*tt);
	printf("size: %d \ntime: %f s\nspeed: %f \n",test_len,tt,speed);

	for (int j=0 ; j<SM3_DIGEST_LENGTH; j++ )
	{
		printf("%hhX,",dgst[j]);
	}
	// printf("%x\n", dgst[0]);
}
