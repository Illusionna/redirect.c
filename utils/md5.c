#include "md5.h"


unsigned int A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
char S[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};


char *md5(char *input) {
    int N = strlen(input);
    int K;
    if (N < 56) K = 0;
    else K = (N - 56) / 64 + 1;
    char M = 64 * K + 55 - N;

    // Convert bytes to decimal arrays.
    short *omega = (short *)malloc(sizeof(short) * (N + 1 + M + 8));
    for (int i = 0; input[i] != '\0'; ++i) {
        if (input[i] < 0) omega[i] = input[i] + 256;
        else omega[i] = input[i];
    }

    // Zero bit padding.
    omega[N] = 128;
    for (int i = 0; i < M; ++i)  omega[N + 1 + i] = 0;

    // Effective information storage padding.
    int tmp = N << 3;
    for (int i = 0; i < 8; ++i) {
        omega[N + 1 + M + i] = tmp & 0xff;
        tmp = tmp >> 8;
    }

    // Encryption iteration.
    for (int i = 0; i < K + 1; ++i) {
        int k;
        unsigned int tmp;
        unsigned int Xk;
        unsigned int AA = A, BB = B, CC = C, DD = D;
        for (int j = 0; j < 64; ++j) {
            unsigned int aa = DD, bb = AA, cc = BB, dd = CC;
            unsigned int a = AA, b = BB, c = CC, d = DD;
            if ((0 <= j) && (j <= 15)) {
                k = i * 64 + j * 4;
                Xk = MathcalH(omega[k], omega[k + 1], omega[k + 2], omega[k + 3]);
                tmp = (a + (F(b, c, d) & 0xffffffff) + Xk + T(j + 1)) & 0xffffffff;
            } else if ((16 <= j) && (j <= 31)) {
                k = i * 64 + (((5 * j) + 1) % 16) * 4;
                Xk = MathcalH(omega[k], omega[k + 1], omega[k + 2], omega[k + 3]);
                tmp = (a + (G(b, c, d) & 0xffffffff) + Xk + T(j + 1)) & 0xffffffff;
            } else if ((32 <= j) && (j <= 47)) {
                k = i * 64 + (((3 * j) + 5) % 16) * 4;
                Xk = MathcalH(omega[k], omega[k + 1], omega[k + 2], omega[k + 3]);
                tmp = (a + (H(b, c, d) & 0xffffffff) + Xk + T(j + 1)) & 0xffffffff;
            } else {
                k = i * 64 + ((7 * j) % 16) * 4;
                Xk = MathcalH(omega[k], omega[k + 1], omega[k + 2], omega[k + 3]);
                tmp = (a + (I(b, c, d) & 0xffffffff) + Xk + T(j + 1)) & 0xffffffff;
            }
            bb = b + L(tmp, S[j]);
            AA = aa; BB = bb; CC = cc; DD = dd;
        }
        A = (A + AA) & 0xffffffff; B = (B + BB) & 0xffffffff;
        C = (C + CC) & 0xffffffff; D = (D + DD) & 0xffffffff;
    }

    free(omega);
    char *result = (char *)malloc(sizeof(char) * 64);
    snprintf(result, 64, "%x%x%x%x", Swap(A), Swap(B), Swap(C), Swap(D));
    return result;
}



char *truncate_string(char *digest, int n) {
    if (digest == NULL) return NULL;
    if (n >= strlen(digest)) return digest;
    digest[n] = '\0';
    return digest;
}