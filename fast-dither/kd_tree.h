#pragma once

#include <cstddef>
#include <cstdint>
#include <nanoflann.hpp>

#include <array>
#include <vector>

template <size_t Dim>
using Point = std::array<double, Dim>;

template <size_t Dim>
using Points = std::vector<Point<Dim>>;

template <size_t Dim>
class KdTree {
private:
    struct DataAdapter {
    public:
        inline size_t kdtree_get_point_count() const {
            return points.size();
        }

        inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
            return points[idx][dim];
        }

        template <class BBOX>
        bool kdtree_get_bbox(BBOX&) const {
            return false;
        }

    public:
        Points<Dim> points;
    };

    using NanoflannKdTree =
        nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, DataAdapter>,
                                            DataAdapter, Dim>;

public:
    explicit KdTree(Points<Dim> points) : points_{std::move(points)}, kd_tree_(Dim, points_) {
    }

    size_t GetNearest(const Point<Dim>& point) const {
        uint32_t index;
        double dist;
        kd_tree_.knnSearch(point.data(), 1, &index, &dist);
        return index;
    }

private:
    DataAdapter points_;
    NanoflannKdTree kd_tree_;
};
