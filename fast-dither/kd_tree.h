#pragma once

#include <nanoflann.hpp>

#include <array>
#include <vector>

template <size_t Dim>
using Point = std::array<double, Dim>;

template <size_t Dim>
using Points = std::vector<Point<Dim>>;

template <size_t Dim>
class KdTree {
public:
    explicit KdTree(Points<Dim> points) {
    }

    size_t GetNearest(const Point<Dim>& point) const {
        return 0;
    }
};
