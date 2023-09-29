#pragma once

#include <stdexcept>
#include <vector>
#include <cstddef>
#include <algorithm>

void Swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void Rotate(std::vector<int>* data, size_t shift) {
    size_t rotated_cnt = 0;
    auto& v = *data;
    size_t len = v.size();
    shift = len - shift;

    int tmp_elem;
    for (size_t i = 0; i < shift; ++i) {
        tmp_elem = v[i];
        for (size_t j = (i + shift) % len; j != i; j = (j + shift) % len, ++rotated_cnt) {
            Swap(&v[j], &tmp_elem);
        }

        v[i] = tmp_elem;
        ++rotated_cnt;

        if (rotated_cnt == len) {
            return;
        }
    }
}
