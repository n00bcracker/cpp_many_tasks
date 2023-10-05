#pragma once

#include <cstddef>
#include <vector>

using std::vector;

class Matrix {
    vector<vector<double>> matrix_;
    size_t rows_num_, cols_num_;

public:
    Matrix(size_t rows_num, size_t cols_num)
        : matrix_(rows_num, vector<double>(cols_num, .0)),
          rows_num_(rows_num),
          cols_num_(cols_num) {
    }

    explicit Matrix(size_t rows_num) : Matrix(rows_num, rows_num) {
    }

    Matrix(const vector<vector<double>>& matrix)
        : matrix_(matrix), rows_num_(matrix_.size()), cols_num_(matrix_[0].size()) {
    }

    size_t Rows() const {
        return rows_num_;
    }

    size_t Columns() const {
        return cols_num_;
    }

    double& operator()(size_t row_idx, size_t col_idx) {
        return matrix_[row_idx][col_idx];
    }

    double operator()(size_t row_idx, size_t col_idx) const {
        return matrix_[row_idx][col_idx];
    }

    Matrix& operator+=(const Matrix& rhs) {
        for (size_t i = 0; i < rows_num_; ++i) {
            for (size_t j = 0; j < cols_num_; ++j) {
                matrix_[i][j] += rhs.matrix_[i][j];
            }
        }

        return *this;
    }

    Matrix operator-() const {
        Matrix negative(rows_num_, cols_num_);
        for (size_t i = 0; i < negative.rows_num_; ++i) {
            for (size_t j = 0; j < negative.cols_num_; ++j) {
                negative.matrix_[i][j] = -matrix_[i][j];
            }
        }

        return negative;
    }

    Matrix& operator-=(const Matrix& rhs) {
        return (*this) += -rhs;
    }

    friend Matrix operator*(const Matrix& lhs, const Matrix& rhs);

    Matrix& operator*=(const Matrix& rhs) {
        return *this = *this * rhs;
    }
};

Matrix operator+(Matrix lhs, const Matrix& rhs) {
    return lhs += rhs;
}

Matrix operator-(Matrix lhs, const Matrix& rhs) {
    return lhs -= rhs;
}

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    Matrix dot(lhs.rows_num_, rhs.cols_num_);
    for (size_t i = 0; i < dot.rows_num_; ++i) {
        for (size_t j = 0; j < dot.cols_num_; ++j) {
            double sum = 0;
            for (size_t z = 0; z < lhs.cols_num_; ++z) {
                sum += lhs.matrix_[i][z] * rhs.matrix_[z][j];
            }
            dot.matrix_[i][j] = sum;
        }
    }

    return dot;
}

Matrix Transpose(const Matrix& m) {
    Matrix result(m.Columns(), m.Rows());
    for (size_t i = 0; i < result.Rows(); ++i) {
        for (size_t j = 0; j < result.Columns(); ++j) {
            result(i, j) = m(j, i);
        }
    }

    return result;
}

Matrix Identity(size_t rows_num) {
    Matrix result(rows_num);
    for (size_t i = 0; i < result.Rows(); ++i) {
        result(i, i) = 1;
    }

    return result;
}