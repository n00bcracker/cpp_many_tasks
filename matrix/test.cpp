#include <catch.hpp>
#include <matrix.h>
#include <ranges>

bool EqualMatrix(const Matrix& a, const Matrix& b) {
    if (a.Rows() != b.Rows()) {
        return false;
    }
    if (a.Columns() != b.Columns()) {
        return false;
    }
    for (auto i = 0ul; i < a.Rows(); ++i) {
        for (auto j = 0ul; j < a.Columns(); ++j) {
            if (a(i, j) != Approx(b(i, j))) {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Constructors") {
    {
        Matrix a(3);
        REQUIRE(3u == a.Rows());
        REQUIRE(3u == a.Columns());
    }
    {
        Matrix a(3, 5);
        REQUIRE(3u == a.Rows());
        REQUIRE(5u == a.Columns());
    }
    {
        Matrix a({{1, 2}, {3, 4}, {5, 6}});
        REQUIRE(3u == a.Rows());
        REQUIRE(2u == a.Columns());
        REQUIRE(3 == a(1, 0));
    }
}

TEST_CASE("Constness") {
    {
        Matrix a({{1, 2}, {3, 4}});
        const auto& b = a;
        REQUIRE(2u == b.Rows());
        REQUIRE(2u == b.Columns());
        REQUIRE(2 == b(0, 1));
    }
    {
        const auto first = Identity(3);
        const Matrix second(3);
        REQUIRE(true == EqualMatrix(first, Transpose(first)));
        REQUIRE(true == EqualMatrix(second, first - first));
        REQUIRE(true == EqualMatrix(first, first * first));
    }
}

TEST_CASE("Operations") {
    Matrix a({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    Matrix b({{0.0, 1.0, 0.0}, {1.0, 1.0, 2.0}});
    Matrix c({{-1.0, -1.0}, {1.0, 1.0}, {1.0, -1.0}});

    REQUIRE(!EqualMatrix(a, Transpose(a)));
    REQUIRE(EqualMatrix(Transpose(a), Matrix({{1.0, 4.0}, {2.0, 5.0}, {3.0, 6.0}})));

    auto old_a = a;
    REQUIRE(EqualMatrix(a += b, Matrix({{1.0, 3.0, 3.0}, {5.0, 6.0, 8.0}})));
    REQUIRE(EqualMatrix(a -= b, old_a));
    REQUIRE(EqualMatrix(a -= a, Matrix(2, 3)));

    REQUIRE(EqualMatrix(b * c, Matrix({{1.0, 1.0}, {2.0, -2.0}})));
}
