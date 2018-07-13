#include "sm3fast.h"
#define ROTATELEFT(X,n)  (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )

void sm3_compress(uint32_t digest[8], const unsigned char block[64]){
  int j;
	uint32_t W[68], W1[64];
	const uint32_t *pblock = (const uint32_t *)block;

  for (j = 0; j < 16; j++) {
    W[j] = cpu_to_be32(pblock[j]);
  }

	uint32_t A = digest[0];
	uint32_t B = digest[1];
	uint32_t C = digest[2];
	uint32_t D = digest[3];
	uint32_t E = digest[4];
	uint32_t F = digest[5];
	uint32_t G = digest[6];
	uint32_t H = digest[7];
	// uint32_t SS1,SS2,TT1,TT2,T[64];

  uint32_t SS1,SS2,TT1,TT2,T;
  uint32_t t[64] ={
    0x79cc4519,0xf3988a32,0x1e7311465,0x3ce6228cb,0x79cc45197,0xf3988a32f,0x1e7311465e,
    0x3ce6228cbc,0x79cc451979,0xf3988a32f3,0x1e7311465e7,0x3ce6228cbce,0x79cc451979c,
    0xf3988a32f39,0x1e7311465e73,0x3ce6228cbce6,0x7a879d8a7a87,0xf50f3b14f50f,0x1ea1e7629ea1e,
    0x3d43cec53d43c,0x7a879d8a7a879,0xf50f3b14f50f3,0x1ea1e7629ea1e7,0x3d43cec53d43ce,
    0x7a879d8a7a879d,0xf50f3b14f50f3b,0x1ea1e7629ea1e76,0x3d43cec53d43cec,0x7a879d8a7a879d8,
    0xf50f3b14f50f3b1,0x1ea1e7629ea1e762,0x3d43cec53d43cec5,0x7a879d8a,0xf50f3b14,0x1ea1e7629,
    0x3d43cec53,0x7a879d8a7,0xf50f3b14f,0x1ea1e7629e,0x3d43cec53d,0x7a879d8a7a,0xf50f3b14f5,
    0x1ea1e7629ea,0x3d43cec53d4,0x7a879d8a7a8,0xf50f3b14f50,0x1ea1e7629ea1,0x3d43cec53d43,0x7a879d8a7a87,
    0xf50f3b14f50f,0x1ea1e7629ea1e,0x3d43cec53d43c,0x7a879d8a7a879,0xf50f3b14f50f3,0x1ea1e7629ea1e7,0x3d43cec53d43ce,
    0x7a879d8a7a879d,0xf50f3b14f50f3b,0x1ea1e7629ea1e76,0x3d43cec53d43cec,0x7a879d8a7a879d8,0xf50f3b14f50f3b1,
    0x1ea1e7629ea1e762,0x3d43cec53d43cec5};

    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[0];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF0(A,B,C) + TT2 + (W[0] ^ W[4]);
    H = H + GG0(E,F,G) + TT1 + W[0];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[1];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF0(D,A,B) + TT2 + (W[1] ^ W[5]);
    G = G + GG0(H,E,F) + TT1 + W[1];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[2];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF0(C,D,A) + TT2 + (W[2] ^ W[6]);
    F = F + GG0(G,H,E) + TT1 + W[2];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[3];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF0(B,C,D) + TT2 + (W[3] ^ W[7]);
    E = E + GG0(F,G,H) + TT1 + W[3];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[4];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF0(A,B,C) + TT2 + (W[4] ^ W[8]);
    H = H + GG0(E,F,G) + TT1 + W[4];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[5];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF0(D,A,B) + TT2 + (W[5] ^ W[9]);
    G = G + GG0(H,E,F) + TT1 + W[5];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[6];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF0(C,D,A) + TT2 + (W[6] ^ W[10]);
    F = F + GG0(G,H,E) + TT1 + W[6];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[7];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF0(B,C,D) + TT2 + (W[7] ^ W[11]);
    E = E + GG0(F,G,H) + TT1 + W[7];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[8];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF0(A,B,C) + TT2 + (W[8] ^ W[12]);
    H = H + GG0(E,F,G) + TT1 + W[8];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[9];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF0(D,A,B) + TT2 + (W[9] ^ W[13]);
    G = G + GG0(H,E,F) + TT1 + W[9];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[10];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF0(C,D,A) + TT2 + (W[10] ^ W[14]);
    F = F + GG0(G,H,E) + TT1 + W[10];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[11];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF0(B,C,D) + TT2 + (W[11] ^ W[15]);
    E = E + GG0(F,G,H) + TT1 + W[11];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[16] = P1( W[0] ^ W[7] ^ ROTATELEFT(W[13],15)) ^ ROTATELEFT(W[3],7 ) ^ W[10];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[12];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF0(A,B,C) + TT2 + (W[12] ^ W[16]);
    H = H + GG0(E,F,G) + TT1 + W[12];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[17] = P1( W[1] ^ W[8] ^ ROTATELEFT(W[14],15)) ^ ROTATELEFT(W[4],7 ) ^ W[11];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[13];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF0(D,A,B) + TT2 + (W[13] ^ W[17]);
    G = G + GG0(H,E,F) + TT1 + W[13];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[18] = P1( W[2] ^ W[9] ^ ROTATELEFT(W[15],15)) ^ ROTATELEFT(W[5],7 ) ^ W[12];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[14];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF0(C,D,A) + TT2 + (W[14] ^ W[18]);
    F = F + GG0(G,H,E) + TT1 + W[14];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[19] = P1( W[3] ^ W[10] ^ ROTATELEFT(W[16],15)) ^ ROTATELEFT(W[6],7 ) ^ W[13];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[15];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF0(B,C,D) + TT2 + (W[15] ^ W[19]);
    E = E + GG0(F,G,H) + TT1 + W[15];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[20] = P1( W[4] ^ W[11] ^ ROTATELEFT(W[17],15)) ^ ROTATELEFT(W[7],7 ) ^ W[14];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[16];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[16] ^ W[20]);
    H = H + GG1(E,F,G) + TT1 + W[16];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[21] = P1( W[5] ^ W[12] ^ ROTATELEFT(W[18],15)) ^ ROTATELEFT(W[8],7 ) ^ W[15];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[17];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[17] ^ W[21]);
    G = G + GG1(H,E,F) + TT1 + W[17];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[22] = P1( W[6] ^ W[13] ^ ROTATELEFT(W[19],15)) ^ ROTATELEFT(W[9],7 ) ^ W[16];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[18];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[18] ^ W[22]);
    F = F + GG1(G,H,E) + TT1 + W[18];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[23] = P1( W[7] ^ W[14] ^ ROTATELEFT(W[20],15)) ^ ROTATELEFT(W[10],7 ) ^ W[17];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[19];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[19] ^ W[23]);
    E = E + GG1(F,G,H) + TT1 + W[19];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[24] = P1( W[8] ^ W[15] ^ ROTATELEFT(W[21],15)) ^ ROTATELEFT(W[11],7 ) ^ W[18];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[20];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[20] ^ W[24]);
    H = H + GG1(E,F,G) + TT1 + W[20];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[25] = P1( W[9] ^ W[16] ^ ROTATELEFT(W[22],15)) ^ ROTATELEFT(W[12],7 ) ^ W[19];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[21];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[21] ^ W[25]);
    G = G + GG1(H,E,F) + TT1 + W[21];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[26] = P1( W[10] ^ W[17] ^ ROTATELEFT(W[23],15)) ^ ROTATELEFT(W[13],7 ) ^ W[20];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[22];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[22] ^ W[26]);
    F = F + GG1(G,H,E) + TT1 + W[22];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[27] = P1( W[11] ^ W[18] ^ ROTATELEFT(W[24],15)) ^ ROTATELEFT(W[14],7 ) ^ W[21];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[23];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[23] ^ W[27]);
    E = E + GG1(F,G,H) + TT1 + W[23];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[28] = P1( W[12] ^ W[19] ^ ROTATELEFT(W[25],15)) ^ ROTATELEFT(W[15],7 ) ^ W[22];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[24];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[24] ^ W[28]);
    H = H + GG1(E,F,G) + TT1 + W[24];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[29] = P1( W[13] ^ W[20] ^ ROTATELEFT(W[26],15)) ^ ROTATELEFT(W[16],7 ) ^ W[23];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[25];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[25] ^ W[29]);
    G = G + GG1(H,E,F) + TT1 + W[25];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[30] = P1( W[14] ^ W[21] ^ ROTATELEFT(W[27],15)) ^ ROTATELEFT(W[17],7 ) ^ W[24];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[26];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[26] ^ W[30]);
    F = F + GG1(G,H,E) + TT1 + W[26];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[31] = P1( W[15] ^ W[22] ^ ROTATELEFT(W[28],15)) ^ ROTATELEFT(W[18],7 ) ^ W[25];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[27];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[27] ^ W[31]);
    E = E + GG1(F,G,H) + TT1 + W[27];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[32] = P1( W[16] ^ W[23] ^ ROTATELEFT(W[29],15)) ^ ROTATELEFT(W[19],7 ) ^ W[26];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[28];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[28] ^ W[32]);
    H = H + GG1(E,F,G) + TT1 + W[28];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[33] = P1( W[17] ^ W[24] ^ ROTATELEFT(W[30],15)) ^ ROTATELEFT(W[20],7 ) ^ W[27];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[29];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[29] ^ W[33]);
    G = G + GG1(H,E,F) + TT1 + W[29];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[34] = P1( W[18] ^ W[25] ^ ROTATELEFT(W[31],15)) ^ ROTATELEFT(W[21],7 ) ^ W[28];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[30];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[30] ^ W[34]);
    F = F + GG1(G,H,E) + TT1 + W[30];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[35] = P1( W[19] ^ W[26] ^ ROTATELEFT(W[32],15)) ^ ROTATELEFT(W[22],7 ) ^ W[29];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[31];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[31] ^ W[35]);
    E = E + GG1(F,G,H) + TT1 + W[31];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[36] = P1( W[20] ^ W[27] ^ ROTATELEFT(W[33],15)) ^ ROTATELEFT(W[23],7 ) ^ W[30];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[32];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[32] ^ W[36]);
    H = H + GG1(E,F,G) + TT1 + W[32];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[37] = P1( W[21] ^ W[28] ^ ROTATELEFT(W[34],15)) ^ ROTATELEFT(W[24],7 ) ^ W[31];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[33];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[33] ^ W[37]);
    G = G + GG1(H,E,F) + TT1 + W[33];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[38] = P1( W[22] ^ W[29] ^ ROTATELEFT(W[35],15)) ^ ROTATELEFT(W[25],7 ) ^ W[32];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[34];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[34] ^ W[38]);
    F = F + GG1(G,H,E) + TT1 + W[34];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[39] = P1( W[23] ^ W[30] ^ ROTATELEFT(W[36],15)) ^ ROTATELEFT(W[26],7 ) ^ W[33];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[35];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[35] ^ W[39]);
    E = E + GG1(F,G,H) + TT1 + W[35];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[40] = P1( W[24] ^ W[31] ^ ROTATELEFT(W[37],15)) ^ ROTATELEFT(W[27],7 ) ^ W[34];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[36];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[36] ^ W[40]);
    H = H + GG1(E,F,G) + TT1 + W[36];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[41] = P1( W[25] ^ W[32] ^ ROTATELEFT(W[38],15)) ^ ROTATELEFT(W[28],7 ) ^ W[35];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[37];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[37] ^ W[41]);
    G = G + GG1(H,E,F) + TT1 + W[37];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[42] = P1( W[26] ^ W[33] ^ ROTATELEFT(W[39],15)) ^ ROTATELEFT(W[29],7 ) ^ W[36];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[38];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[38] ^ W[42]);
    F = F + GG1(G,H,E) + TT1 + W[38];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[43] = P1( W[27] ^ W[34] ^ ROTATELEFT(W[40],15)) ^ ROTATELEFT(W[30],7 ) ^ W[37];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[39];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[39] ^ W[43]);
    E = E + GG1(F,G,H) + TT1 + W[39];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[44] = P1( W[28] ^ W[35] ^ ROTATELEFT(W[41],15)) ^ ROTATELEFT(W[31],7 ) ^ W[38];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[40];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[40] ^ W[44]);
    H = H + GG1(E,F,G) + TT1 + W[40];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[45] = P1( W[29] ^ W[36] ^ ROTATELEFT(W[42],15)) ^ ROTATELEFT(W[32],7 ) ^ W[39];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[41];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[41] ^ W[45]);
    G = G + GG1(H,E,F) + TT1 + W[41];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[46] = P1( W[30] ^ W[37] ^ ROTATELEFT(W[43],15)) ^ ROTATELEFT(W[33],7 ) ^ W[40];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[42];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[42] ^ W[46]);
    F = F + GG1(G,H,E) + TT1 + W[42];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[47] = P1( W[31] ^ W[38] ^ ROTATELEFT(W[44],15)) ^ ROTATELEFT(W[34],7 ) ^ W[41];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[43];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[43] ^ W[47]);
    E = E + GG1(F,G,H) + TT1 + W[43];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[48] = P1( W[32] ^ W[39] ^ ROTATELEFT(W[45],15)) ^ ROTATELEFT(W[35],7 ) ^ W[42];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[44];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[44] ^ W[48]);
    H = H + GG1(E,F,G) + TT1 + W[44];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[49] = P1( W[33] ^ W[40] ^ ROTATELEFT(W[46],15)) ^ ROTATELEFT(W[36],7 ) ^ W[43];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[45];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[45] ^ W[49]);
    G = G + GG1(H,E,F) + TT1 + W[45];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[50] = P1( W[34] ^ W[41] ^ ROTATELEFT(W[47],15)) ^ ROTATELEFT(W[37],7 ) ^ W[44];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[46];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[46] ^ W[50]);
    F = F + GG1(G,H,E) + TT1 + W[46];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[51] = P1( W[35] ^ W[42] ^ ROTATELEFT(W[48],15)) ^ ROTATELEFT(W[38],7 ) ^ W[45];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[47];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[47] ^ W[51]);
    E = E + GG1(F,G,H) + TT1 + W[47];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[52] = P1( W[36] ^ W[43] ^ ROTATELEFT(W[49],15)) ^ ROTATELEFT(W[39],7 ) ^ W[46];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[48];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[48] ^ W[52]);
    H = H + GG1(E,F,G) + TT1 + W[48];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[53] = P1( W[37] ^ W[44] ^ ROTATELEFT(W[50],15)) ^ ROTATELEFT(W[40],7 ) ^ W[47];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[49];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[49] ^ W[53]);
    G = G + GG1(H,E,F) + TT1 + W[49];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[54] = P1( W[38] ^ W[45] ^ ROTATELEFT(W[51],15)) ^ ROTATELEFT(W[41],7 ) ^ W[48];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[50];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[50] ^ W[54]);
    F = F + GG1(G,H,E) + TT1 + W[50];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[55] = P1( W[39] ^ W[46] ^ ROTATELEFT(W[52],15)) ^ ROTATELEFT(W[42],7 ) ^ W[49];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[51];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[51] ^ W[55]);
    E = E + GG1(F,G,H) + TT1 + W[51];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[56] = P1( W[40] ^ W[47] ^ ROTATELEFT(W[53],15)) ^ ROTATELEFT(W[43],7 ) ^ W[50];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[52];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[52] ^ W[56]);
    H = H + GG1(E,F,G) + TT1 + W[52];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[57] = P1( W[41] ^ W[48] ^ ROTATELEFT(W[54],15)) ^ ROTATELEFT(W[44],7 ) ^ W[51];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[53];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[53] ^ W[57]);
    G = G + GG1(H,E,F) + TT1 + W[53];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[58] = P1( W[42] ^ W[49] ^ ROTATELEFT(W[55],15)) ^ ROTATELEFT(W[45],7 ) ^ W[52];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[54];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[54] ^ W[58]);
    F = F + GG1(G,H,E) + TT1 + W[54];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[59] = P1( W[43] ^ W[50] ^ ROTATELEFT(W[56],15)) ^ ROTATELEFT(W[46],7 ) ^ W[53];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[55];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[55] ^ W[59]);
    E = E + GG1(F,G,H) + TT1 + W[55];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[60] = P1( W[44] ^ W[51] ^ ROTATELEFT(W[57],15)) ^ ROTATELEFT(W[47],7 ) ^ W[54];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[56];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[56] ^ W[60]);
    H = H + GG1(E,F,G) + TT1 + W[56];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[61] = P1( W[45] ^ W[52] ^ ROTATELEFT(W[58],15)) ^ ROTATELEFT(W[48],7 ) ^ W[55];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[57];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[57] ^ W[61]);
    G = G + GG1(H,E,F) + TT1 + W[57];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[62] = P1( W[46] ^ W[53] ^ ROTATELEFT(W[59],15)) ^ ROTATELEFT(W[49],7 ) ^ W[56];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[58];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[58] ^ W[62]);
    F = F + GG1(G,H,E) + TT1 + W[58];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[63] = P1( W[47] ^ W[54] ^ ROTATELEFT(W[60],15)) ^ ROTATELEFT(W[50],7 ) ^ W[57];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[59];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[59] ^ W[63]);
    E = E + GG1(F,G,H) + TT1 + W[59];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);

    W[64] = P1( W[48] ^ W[55] ^ ROTATELEFT(W[61],15)) ^ ROTATELEFT(W[51],7 ) ^ W[58];
    TT2 = ROTATELEFT(A,12);
    TT1 = TT2 + E + t[60];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    D = D + FF1(A,B,C) + TT2 + (W[60] ^ W[64]);
    H = H + GG1(E,F,G) + TT1 + W[60];
    B = ROTATELEFT(B, 9);
    F = ROTATELEFT(F ,19);
    H = P0(H);

    W[65] = P1( W[49] ^ W[56] ^ ROTATELEFT(W[62],15)) ^ ROTATELEFT(W[52],7 ) ^ W[59];
    TT2 = ROTATELEFT(D,12);
    TT1 = TT2 + H + t[61];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    C = C + FF1(D,A,B) + TT2 + (W[61] ^ W[65]);
    G = G + GG1(H,E,F) + TT1 + W[61];
    A = ROTATELEFT(A, 9);
    E = ROTATELEFT(E ,19);
    G = P0(G);

    W[66] = P1( W[50] ^ W[57] ^ ROTATELEFT(W[63],15)) ^ ROTATELEFT(W[53],7 ) ^ W[60];
    TT2 = ROTATELEFT(C,12);
    TT1 = TT2 + G + t[62];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    B = B + FF1(C,D,A) + TT2 + (W[62] ^ W[66]);
    F = F + GG1(G,H,E) + TT1 + W[62];
    D = ROTATELEFT(D, 9);
    H = ROTATELEFT(H ,19);
    F = P0(F);

    W[67] = P1( W[51] ^ W[58] ^ ROTATELEFT(W[64],15)) ^ ROTATELEFT(W[54],7 ) ^ W[61];
    TT2 = ROTATELEFT(B,12);
    TT1 = TT2 + F + t[63];
    TT1 = ROTATELEFT(TT1,7);
    TT2 ^= TT1;
    A = A + FF1(B,C,D) + TT2 + (W[63] ^ W[67]);
    E = E + GG1(F,G,H) + TT1 + W[63];
    C = ROTATELEFT(C, 9);
    G = ROTATELEFT(G ,19);
    E = P0(E);


  digest[0] ^= A;
	digest[1] ^= B;
	digest[2] ^= C;
	digest[3] ^= D;
	digest[4] ^= E;
	digest[5] ^= F;
	digest[6] ^= G;
	digest[7] ^= H;
}
