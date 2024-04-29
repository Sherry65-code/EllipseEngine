#include "Maths.h"

uint32_t _eClamp(uint32_t num, uint32_t min, uint32_t max) {
    if (num > max) return max;
    if (num < min) return min;
    return num;
}