#pragma once

#include <stdexcept>

int BinPow(int a, int64_t b, int c) {
    uint64_t res = 1;
    do {
        if (b % 2) {
            res = res * a % c;
        }
        a = static_cast<uint64_t>(a) * a % c;
        b /= 2;
    } while (b);

    return res;
}
