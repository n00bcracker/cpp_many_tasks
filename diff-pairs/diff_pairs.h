#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <map>

int64_t CountPairs(const std::vector<int>& data, int x) {
    std::map<int64_t, std::vector<int>> positions;
    for (int i = 0; i < std::ssize(data); ++i) {
        positions[data[i]].push_back(i);
    }

    int64_t res = 0;
    int64_t search_value;
    for (const auto& [num, poses] : positions) {
        search_value = static_cast<int64_t>(x) - num;
        if (search_value > num) {
            if (positions.contains(search_value)) {
                const auto& sv_poses = positions[search_value];
                res += std::ssize(poses) * std::ssize(sv_poses);
            }
        } else if (search_value == num) {
            res += std::ssize(poses) * (std::ssize(poses) - 1) / 2;
        } else {
            break;
        }
    }

    return res;
}
