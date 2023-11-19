#include <ugly.h>
#include <bad.h>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

class Spline::SplineImpl {
public:
    SplineImpl(std::vector<double>&& x, std::vector<double>&& y, double a, double b)
        : x_(std::move(x)), y_(std::move(y)), y2_(x_.size()) {
        int res = mySplineSnd(x_.data(), y_.data(), x_.size(), a, b, y2_.data());
        if (res == -1) {
            throw std::bad_alloc();
        } else if (res == -2) {
            throw std::invalid_argument("x is not monitonic increasing");
        }
    }

    double Interpolate(double x) const {
        double f;
        int res = mySplintCube(x_.data(), y_.data(), y2_.data(), x_.size(), x, &f);
        if (res == -1) {
            throw std::runtime_error("Array length is 0");
        }

        return f;
    }

private:
    const std::vector<double> x_;
    const std::vector<double> y_;
    std::vector<double> y2_;
};

Spline::Spline(std::vector<double> x, std::vector<double> y, double a, double b)
    : impl_(std::make_unique<SplineImpl>(SplineImpl(std::move(x), std::move(y), a, b))) {
}

Spline::~Spline() {
}

double Spline::Interpolate(double x) const {
    return impl_->Interpolate(x);
}