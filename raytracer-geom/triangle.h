#pragma once

#include <vector.h>
#include <array>

class Triangle {
public:
    Triangle(const Vector& a, const Vector& b, const Vector& c) : triangle_{a, b, c} {
    }

    const Vector& operator[](size_t ind) const {
        return triangle_[ind];
    }

    double Area() const {
        Vector v1, v2;
        v1 = triangle_[1] - triangle_[0];
        v2 = triangle_[2] - triangle_[0];
        double area = Length(CrossProduct(v1, v2)) / 2;
        return area;
    }

private:
    std::array<Vector, 3> triangle_;
};
