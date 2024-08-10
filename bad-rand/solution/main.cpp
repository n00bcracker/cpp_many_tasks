
#include <sys/types.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <limits>
#include "util.h"

using std::vector;

const std::string kAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

class Matrix {
    vector<vector<int32_t>> matrix_;
    size_t rows_num_, cols_num_;

public:
    Matrix(size_t rows_num, size_t cols_num)
        : matrix_(rows_num, vector<int32_t>(cols_num, 0)),
          rows_num_(rows_num),
          cols_num_(cols_num) {
    }

    explicit Matrix(size_t rows_num) : Matrix(rows_num, rows_num) {
    }

    size_t Rows() const {
        return rows_num_;
    }

    size_t Columns() const {
        return cols_num_;
    }

    int32_t& operator()(size_t row_idx, size_t col_idx) {
        return matrix_[row_idx][col_idx];
    }

    int32_t operator()(size_t row_idx, size_t col_idx) const {
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

    Matrix Pow(size_t pow) const;

    void Print() const {
        std::cout << "[";
        for (size_t i = 0; i < rows_num_; ++i) {
            std::cout << "[";
            for (size_t j = 0; j < cols_num_; ++j) {
                std::cout << matrix_[i][j];
                if (j + 1 != cols_num_) {
                    std::cout << ", ";
                }
            }
            std::cout << "]";
            if (i + 1 != rows_num_) {
                std::cout << std::endl;
            }
        }
         std::cout << "]" << std::endl;
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
            int32_t sum = 0;
            for (size_t z = 0; z < lhs.cols_num_; ++z) {
                sum += static_cast<uint32_t>(lhs.matrix_[i][z] * rhs.matrix_[z][j]);
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

Matrix Matrix::Pow(size_t pow) const {
    Matrix res = Identity(rows_num_);
    Matrix m = *this;
    do {
        if (pow & 1) {
            res *= m;
        }
        m *= m;
        pow = pow >> 1;
    } while (pow);

    return res;
}

void GenString(std::string* s) {
    for (auto& c : *s) {
        c = kAlphabet[std::rand() % kAlphabet.size()];
    }
}

Matrix MakeState(uint32_t seed) {
    Matrix state(1, 31);
    state(0, 0) = seed;

    int32_t word = seed;
    for (size_t i = 1; i < 31; ++i) {
        int64_t hi = word / 127773;
        int64_t lo = word % 127773;
        word = 16807 * lo - 2836 * hi;
        if (word < 0) {
	        word += 2147483647;
        }
        state(0, i) = word;
    }

    return state;
}

Matrix MakeTransformMatrix() {
    size_t size = 31;
    Matrix transform = Identity(size);
    size_t shift = 3;
    for (size_t j = 0; j < size; ++j) {
        size_t col_idx = (j + shift) % size;
        for (size_t i = 0; i < size; ++i) {
            transform(i, col_idx) += transform(i, j);
        }
    }

    return transform;
}

std::string GenPassword(int32_t seed) {
    static constexpr std::string kIdealPassword = "NLXGI4NoAp";

    Matrix state = MakeState(seed);

    const static Matrix kTransform = MakeTransformMatrix();

    static constexpr size_t kIter = 100'000'001;

    static constexpr size_t kPow = (kIter - 1) / 31 + 1 + 10;
    static constexpr size_t kPos = ((kIter - 1) % 31 + 3) % 31;

    static const Matrix kRandomTransform = kTransform.Pow(kPow);
    Matrix result = state * kRandomTransform;

    std::string password;
    for (size_t i = 0; i < 10; ++i) {
        uint32_t u_random_value = result(0, kPos + i);
        int32_t random_value = u_random_value >> 1;
        char letter = kAlphabet[random_value % kAlphabet.size()];
        password.push_back(letter);
        // if (letter != kIdealPassword[i]) {
        //     return false;
        // }
    }

    return password;
}

int main() {
    const auto current_dir = GetFileDir(__FILE__);
    std::filesystem::path input_file(current_dir / "../seed.txt");
    std::ofstream myfile(input_file);

    // size_t checked_cnt = 0;
    // for (int32_t seed = std::numeric_limits<int32_t>::min(); seed <= std::numeric_limits<int32_t>::max(); ++seed) {
    //     if (seed != 0) {
    //         if (GenPassword(seed)) {
    //             std::cout << "Ideal seed: " << seed << std::endl;
    //             myfile << seed << std::endl;
    //             myfile.close();
    //         }
    //         ++checked_cnt;
    //     }

    //     if (checked_cnt % 1000000 == 0) {
    //         std::cout << "Checked: " << checked_cnt << std::endl;
    //     }
    // }

    std::cout << "My password: " << GenPassword(1543750888) << std::endl;
}
