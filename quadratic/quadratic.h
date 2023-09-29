#pragma once

#include <stdexcept>
#include <cmath>
#include <algorithm>

enum class RootCount { kZero, kOne, kTwo, kInf };

struct Roots {
    RootCount count;
    double first;
    double second;
};

Roots SolveQuadratic(int a, int b, int c) {
    Roots res;
    if (a != 0) {
        const int64_t descriminant = static_cast<int64_t>(b) * b - 4 * static_cast<int64_t>(a) * c;
        if (descriminant < 0) {
            res.count = RootCount::kZero;
        } else if (descriminant == 0) {
            res.count = RootCount::kOne;
            res.first = static_cast<double>(-b) / (2 * a);
        } else {
            res.count = RootCount::kTwo;
            res.first =
                (static_cast<double>(-b) - std::sqrt(static_cast<double>(descriminant))) / (2 * a);
            res.second =
                (static_cast<double>(-b) + std::sqrt(static_cast<double>(descriminant))) / (2 * a);

            if (a < 0) {
                std::swap(res.first, res.second);
            }
        }
    } else {
        if (b != 0) {
            res.count = RootCount::kOne;
            res.first = static_cast<double>(-c) / b;
        } else {
            if (c == 0) {
                res.count = RootCount::kInf;
            } else {
                res.count = RootCount::kZero;
            }
        }
    }

    return res;
}
