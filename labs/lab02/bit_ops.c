#include <stdio.h>
#include "bit_ops.h"

// Return the nth bit of x.
// Assume 0 <= n <= 31
unsigned get_bit(unsigned x,
                 unsigned n) {
    unsigned ret_val = ((x >> n) & 1);

    return ret_val;
}
// Set the nth bit of the value of x to v.
// Assume 0 <= n <= 31, and v is 0 or 1
void set_bit(unsigned * x,
             unsigned n,
             unsigned v) {
    unsigned mask = ~(1 << n); // to clear assigned bit
    (*x) = ((*x) & mask) | (v << n);
}
// Flip the nth bit of the value of x.
// Assume 0 <= n <= 31
void flip_bit(unsigned * x,
              unsigned n) {
   unsigned original_bit = get_bit(*x, n); 
   set_bit(x, n, 1 - original_bit);
    // *x = (*x) ^ (1 << n);
}

