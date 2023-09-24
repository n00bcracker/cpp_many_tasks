#pragma once

#include <string>
#include <stdexcept>
#include <algorithm>

std::string LongSumRef(std::reference_wrapper<const std::string> sr1,
                       std::reference_wrapper<const std::string> sr2) {
    if (sr1.get().size() < sr2.get().size()) {
        std::swap(sr1, sr2);
    }
    const std::string& s1 = sr1.get();
    const std::string& s2 = sr2.get();
    std::string res;

    size_t ls1 = s1.size(), ls2 = s2.size();
    int value1, value2, sum;
    int prev_digit = 0;
    for (size_t i = 0; i < ls1; ++i) {
        value1 = s1[ls1 - i - 1] - '0';
        value2 = i < ls2 ? s2[ls2 - i - 1] - '0' : 0;
        sum = value1 + value2 + prev_digit;
        res += static_cast<char>(sum % 10) + '0';
        prev_digit = sum / 10;
    }

    if (prev_digit != 0) {
        res += static_cast<char>(prev_digit) + '0';
    }

    std::reverse(res.begin(), res.end());
    return res;
}

std::string LongSum(const std::string& a, const std::string& b) {
    return LongSumRef(a, b);
}
