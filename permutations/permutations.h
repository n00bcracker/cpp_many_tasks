#pragma once

#include <vector>
#include <stdexcept>
#include <unordered_set>

void NextNumber(std::vector<int>& buf, std::vector<bool>& banned, int length,
                std::vector<std::vector<int>>& res) {
    if (std::ssize(buf) == length) {
        res.push_back(buf);
    } else {
        for (int i = 0; i < length; ++i) {
            if (!banned[i]) {
                buf.push_back(i);
                banned[i] = true;
                NextNumber(buf, banned, length, res);
                banned[i] = false;
                buf.pop_back();
            }
        }
    }
}

std::vector<std::vector<int>> GeneratePermutations(size_t len) {
    std::vector<bool> banned(len);
    std::vector<std::vector<int>> res;
    std::vector<int> buf;

    NextNumber(buf, banned, len, res);
    return res;
}
