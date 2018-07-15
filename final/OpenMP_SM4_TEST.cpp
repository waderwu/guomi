#include "OpenMP_SM4.h"

#define test_blocks (8)

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

    openmp_sm4_encrypt(p, key, c, test_blocks);
    outputChar(c, 16*test_blocks);

    openmp_sm4_decrypt(p, key, c, test_blocks);
    outputChar(p, 16*test_blocks);
}