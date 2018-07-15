#include "Static_SM4.h"

int main()
{
    // plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
    // key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
    // cipher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46

     u1 key[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
     u1 p[SM4_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
     u1 c[SM4_BLOCK_SIZE];

     static_sm4_encrypt(p, key, c);
     outputChar(c, sizeof (c));

     static_sm4_decrypt(c, key, c);
     outputChar(c, sizeof (c));

     forloop (i, 0, 1000000)
     {
     	static_sm4_encrypt(p, key, p);
     }

    outputChar(p, 16);
}