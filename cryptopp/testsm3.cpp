#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;

#include "cryptopp/sm3.h"
#include "cryptopp/md5.h"
using namespace CryptoPP;
using namespace std;

#define msglen (1<<20)

void test(byte * digest, byte *message, int length)
{
	SM3 sm3;
	clock_t t=clock();
	sm3.CalculateDigest(digest,message,length);
	double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (length)/(1024*1024*tt);
	printf("size: %d \ntime: %f s\nspeed: %f \n",length,tt,speed);
}

int main()
{
	cout<<SM3::StaticAlgorithmName()<<endl;

  byte *message =(byte *)malloc(sizeof(byte)*msglen);
	for (int j=0; j<msglen ; j++)
	{
		message[j] = 'a';
	}
	byte digest[32];

	test(digest,message,16);
	test(digest,message,64);
	test(digest,message,256);
	test(digest,message,1024);
	test(digest,message,8192);
	test(digest,message,16384);
	test(digest,message,msglen);

    for(int i=0;i<32;i++)	printf("%02x",digest[i]);
	printf("\n");
}
