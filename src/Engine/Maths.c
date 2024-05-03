#include "Maths.h"

int Maths_Clamp(int num, int min, int max) {
    if (num > max) return max;
    if (num < min) return min;
    return num;
}