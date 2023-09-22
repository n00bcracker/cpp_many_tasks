#pragma once

#include <vector>
#include <stdexcept>
#include <cstdlib>

std::vector<int> Range(int from, int to, int step = 1) {
    size_t v_size;
    if (step != 0 && (to - from) * step > 0) {
        v_size = (abs(to - from) - 1) / abs(step) + 1;
    } else {
        v_size = 0;
    }
    std::vector<int> res = std::vector<int>(v_size);
    for (int i = 0, val = from; i < std::ssize(res); ++i, val += step) {
        res[i] = val;
    }
    return res;
}
