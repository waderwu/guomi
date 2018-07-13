#include "sm4_share.h"

int main(int argc, char * argv[])
{
  InitCUDA();

  char * filename = argv[1];
  char * outfilename = argv[2];
  int fd = open(filename,O_RDONLY);
  int fdw = open(outfilename,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
  if (!fd)
  {
    printf("%s\n", "open file error");
    exit(-1);
  }

  u1 * key;
	cudaMallocManaged(&key,sizeof(u1)*16);
	u4 * rkey;
	cudaMallocManaged(&rkey,sizeof(u4)*(SM4_RND_KEY_SIZE / sizeof(u4)));
  u1 * p;
  cudaMallocManaged(&p,sizeof(u1)*SIZE);

  u1 *c;
  cudaMallocManaged(&c,sizeof(u1)*SIZE);

  u1 key2[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	for (int i=0; i<16; i++)
	{
		key[i] = key2[i];
	}

  SM4_key_schedule<<<1,1>>>( key, rkey );
	cudaDeviceSynchronize();

  int haveread;


  haveread=read(fd,p,SIZE);
  printf("%d\n",haveread);
  clock_t t=clock();
  int blockSize = 512;
  int blocks = haveread/16;
  int numBlocks = (blocks + blockSize -1) /blockSize;
  speed_test_share<<<numBlocks,blockSize>>>(blocks,p,c,rkey);
  cudaDeviceSynchronize();
  double tt = (double)(clock() - t)/CLOCKS_PER_SEC;
  double speed =(double) (haveread)/(1024*1024*tt);
  write(fdw,c,haveread);
  close(fdw);



	printf("time: %f s\nspeed: %f \n",tt,speed);

	// printf("hahahaha\n");

	// for (int j=0 ; j<32; j++)
	// {
	// 	printf("%04x,",rkey[j]);
	// }



}
