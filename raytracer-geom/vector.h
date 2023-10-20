#pragma once

#include <array>
#include <cstddef>
#include <cmath>

class Vector {
public:
    Vector() : data_{0, 0, 0} {
    }

    Vector(double x, double y, double z) : data_{x, y, z} {
    }

    double& operator[](size_t ind) {
        return data_[ind];
    }
    double operator[](size_t ind) const {
        return data_[ind];
    }

    Vector& operator+=(const Vector& v) {
        for (size_t i = 0; i < 3; ++i) {
            data_[i] += v[i];
        }

        return *this;
    }

    Vector operator+(const Vector& v) const {
        Vector res(*this);
        res += v;
        return res;
    }

    Vector& operator+=(double c) {
        for (size_t i = 0; i < 3; ++i) {
            data_[i] += c;
        }

        return *this;
    }

    Vector operator+(double c) const {
        Vector res(*this);
        res += c;
        return res;
    }

    Vector& operator-=(const Vector& v) {
        for (size_t i = 0; i < 3; ++i) {
            data_[i] -= v[i];
        }

        return *this;
    }

    Vector operator-(const Vector& v) const {
        Vector res(*this);
        res -= v;
        return res;
    }

    Vector& operator-=(double c) {
        *this += -c;
        return *this;
    }

    Vector operator-(double c) const {
        Vector res(*this);
        res -= c;
        return res;
    }

    Vector& operator*=(double k) {
        for (size_t i = 0; i < 3; ++i) {
            data_[i] *= k;
        }

        return *this;
    }

    Vector operator*(double k) const {
        Vector res(*this);
        res *= k;
        return res;
    }

    Vector& operator/=(double k) {
        *this *= 1 / k;
        return *this;
    }

    Vector operator/(double k) const {
        Vector res(*this);
        res /= k;
        return res;
    }

    void Normalize();

private:
    std::array<double, 3> data_;
};

Vector operator*(double k, const Vector& vec) {
    return vec * k;
}

double DotProduct(const Vector& a, const Vector& b) {
    double sum = 0;
    for (size_t i = 0; i < 3; ++i) {
        sum += a[i] * b[i];
    }

    return sum;
}

Vector CrossProduct(const Vector& a, const Vector& b) {
    Vector res;
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];

    return res;
}

double Length(const Vector& v) {
    return std::sqrt(DotProduct(v, v));
}

void Vector::Normalize() {
    auto length = Length(*this);
    if (length > 0) {
        for (size_t i = 0; i < 3; ++i) {
            data_[i] /= length;
        }
    }
}
