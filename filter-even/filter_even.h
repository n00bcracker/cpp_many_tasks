#pragma once

#include <vector>
#include <stdexcept>

void Swap(int* a, int* b) {
    if (a != b) {
        int tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

void FilterEven(std::vector<int>* data) {
    auto& v = *data;
    size_t fp, sp;
    for (fp = 0, sp = 0; fp < v.size(); ++fp) {
        if (v[fp] % 2 == 0) {
            if (fp != sp) {
                v[sp] = v[fp];
            }
            ++sp;
        }
    }

    v.erase(v.begin() + sp, v.end());
}
