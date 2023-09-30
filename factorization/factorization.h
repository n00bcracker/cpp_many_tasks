#pragma once

#include <utility>
#include <vector>
#include <stdexcept>

std::vector<std::pair<int64_t, int>> Factorize(int64_t x) {
    std::vector<std::pair<int64_t, int>> res;

    int64_t devisor = 2;
    while (devisor * devisor <= x) {
        int power = 0;
        while (x % devisor == 0) {
            ++power;
            x /= devisor;
        }

        if (power > 0) {
            res.emplace_back(devisor, power);
        }
        ++devisor;
    }

    if (x != 1) {
        res.emplace_back(x, 1);
    }

    return res;
}
