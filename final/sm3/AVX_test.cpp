#include "avxdef.h"
#include "sm3-hash-4way.h"
#include <stdio.h>
#include <malloc.h>
#include <time.h>

char output1[0x50] = { 0 };
char output2[0x50] = { 0 };
char output3[0x50] = { 0 };
char output4[0x50] = { 0 };
char * input1;
char * input2;
char * input3;
char * input4;
void benchmark(int size) {
	input1 = (char*)malloc(2<<20);
	input2 = (char*)malloc(2<<20);
	input3 = (char*)malloc(2<<20);
	input4 = (char*)malloc(2<<20);
	for (int i = 0; i < size; i++) {
		input1[i] = 'a';
		input2[i] = 'a';
		input3[i] = 'a';
		input4[i] = 'a';
	}
	input1[size] = '\x00';
	input2[size] = '\x00';
	input3[size] = '\x00';
	input4[size] = '\x00';
	memset(output1, sizeof(output1), 0);
	memset(output2, sizeof(output2), 0);
	memset(output3, sizeof(output3), 0);
	memset(output4, sizeof(output4), 0);
	clock_t t = clock();
	for (int i = 0; i < 1000; i++) {
		AVX_SM3(input1, input2, input3, input4, output1, output2, output3, output4);
		input1[size] = '\x00';
		input2[size] = '\x00';
		input3[size] = '\x00';
		input4[size] = '\x00';
	}
	double tt = (double)(clock() - t) / (CLOCKS_PER_SEC*1000);
	double speed = (double)(size*4) / (1024 * 1024 * tt);

	free(input1);
	free(input2);
	free(input3);
	free(input4);

	printf("size: %d time: %f s speed: %f MB/s\n", size, tt, speed);

}
void pp(char *buf) {
	for (int i = 0; i < 32; i++) {
		printf("%2x ", buf[i] & 0xff);
	}
	puts("");
}
void AccuracyTest() {
	printf("\nAccuracy test\n");
	input1 = (char*)malloc(8);
	input2 = (char*)malloc(8);
	input3 = (char*)malloc(8);
	input4 = (char*)malloc(8);
	memset(output1, sizeof(output1), 0);
	memset(output2, sizeof(output2), 0);
	memset(output3, sizeof(output3), 0);
	memset(output4, sizeof(output4), 0);
	strcpy(input1, "abc");
	strcpy(input2, "acb");
	strcpy(input3, "bac");
	strcpy(input4, "bca");
	AVX_SM3(input1, input2, input3, input4, output1, output2, output3, output4);
	printf("The output is: \n");
	pp(output1);
	pp(output2);
	pp(output3);
	pp(output4);
	free(input1);
	free(input2);
	free(input3);
	free(input4);

}
void SpeedTest() {
	printf("\nSpeed Test\n");

	benchmark(16);
	benchmark(64);
	benchmark(256);
	benchmark(1024);
	benchmark(8192);
	benchmark(16384);
	benchmark(1048576);
}
int main() {

	AccuracyTest();
	AccuracyTest();
	SpeedTest();

	
}