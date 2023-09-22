#pragma once

#include <vector>
#include <stdexcept>
#include <cstdlib>

std::vector<int> Unique(const std::vector<int>& data) {
    std::vector<int> res;
    for (auto iter = data.begin(); iter != data.end(); ++iter) {
        if (res.empty() || *iter != res.back()) {
            res.push_back(*iter);
        }
    }
    return res;
}
