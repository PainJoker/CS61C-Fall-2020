#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

void transpose_blocking_helper(int bx_start, int by_start, int blocksize, int n, int *dst, int *src) {
    int bx_end = bx_start + blocksize < n ? bx_start + blocksize : n;
    int by_end = by_start + blocksize < n ? by_start + blocksize : n;

    for(int x = bx_start; x < bx_end; x++) {
        for(int y = by_start; y < by_end; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    for (int bx = 0; bx < n; bx += blocksize) {
        for (int by = 0; by < n; by += blocksize) {
            transpose_blocking_helper(bx, by, blocksize, n, dst, src);
        }
    }
}
