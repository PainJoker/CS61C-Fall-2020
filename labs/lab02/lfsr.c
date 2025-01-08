#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lfsr.h"

void lfsr_calculate(uint16_t *reg) {
    uint16_t mask = 1;
    uint16_t bit0 = (*reg) & mask;

    mask <<= 2;
    uint16_t bit2 = ((*reg) & mask) >> 2;

    mask <<= 1;
    uint16_t bit3 = ((*reg) & mask) >> 3;

    mask <<= 2;
    uint16_t bit5 = ((*reg) & mask) >> 5;

    uint16_t added_bit = bit0 ^ bit2 ^ bit3 ^ bit5;
    uint16_t added_number = added_bit << 15;

    *reg = (*reg >> 1) + added_number;
}

