#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

int64_t CountPairs(const std::vector<int>& data, int x) {
    std::unordered_map<int64_t, std::vector<int>> positions;
    for (int i = 0; i < std::ssize(data); ++i) {
        positions[data[i]].push_back(i);
    }

    int64_t res = 0;
    int64_t search_value;
    for (int i = 0; i < std::ssize(data); ++i) {
        search_value = static_cast<int64_t>(x) - data[i];
        if (positions.contains(search_value)) {
            for (int p : positions[search_value]) {
                if (p > i) {
                    ++res;
                }
            }
        }
    }

    return res;
}
