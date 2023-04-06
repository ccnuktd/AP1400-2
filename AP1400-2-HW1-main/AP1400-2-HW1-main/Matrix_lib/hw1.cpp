#include "hw1.h"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>

using std::cout;


bool is_empty(const Matrix &matrix) {
    return matrix.empty();
}

bool equal_zero(double num) {
    return std::abs(num) < 0.000001;
}

Matrix algebra::zeros(size_t n, size_t m) {
    if (n < 0 | m < 0) throw std::logic_error("Logic error");
    return {n, std::vector<double>(m, 0)};
}

Matrix algebra::ones(size_t n, size_t m) {
    if (n < 0 | m < 0) throw std::logic_error("Logic error");
    return {n, std::vector<double>(m, 1)};
}

Matrix algebra::random(size_t n, size_t m, double min, double max) {
    if (n < 0 | m < 0) throw std::logic_error("Logic error");
    if (min > max) throw std::logic_error("Logic error");

    // use mt19937 to obtain seed
    std::random_device rd;
    std::mt19937 int_seed{rd()};
    std::uniform_real_distribution<> double_generator{min, max};
    Matrix random_matrix{zeros(n ,m)};

    // function use to generate nums
    auto gen_num = [&int_seed, &double_generator]() {
        return double_generator(int_seed);
    };

    for (auto& row : random_matrix) {
        std::generate(row.begin(), row.end(), gen_num);
    }
    return random_matrix;
}

void algebra::show(const Matrix &matrix) {
    // 1. set precision as 3
    // 2. set 3 decimal places use set fixed
    // 3. set every element as 8 width

    cout << std::setprecision(3);
    cout << std::fixed;

    const int width = 8;

    for (auto& row: matrix) {
        for (auto ele : row) {
            cout << std::setw(width) << ele;
        }
        cout << std::endl;
    }
}

Matrix algebra::multiply(const Matrix& matrix, double c) {
    Matrix new_matrix{matrix};
    for (auto& row : new_matrix) {
        for (auto& ele : row) {
            ele = ele * c;
        }
    }
    return new_matrix;
}

bool check_multiply(const Matrix& matrix1, const Matrix& matrix2) {
    if (is_empty(matrix1) && is_empty(matrix2)) return true;
    if (matrix1.empty() | matrix2.empty() | matrix1[0].size() != matrix2.size()) return false;
    return true;
}

Matrix algebra::multiply(const Matrix& matrix1, const Matrix& matrix2) {
    if (!check_multiply(matrix1, matrix2)) throw std::logic_error("Logic error");
    if (is_empty(matrix1) && is_empty(matrix2)) return Matrix{};

    int n = matrix1.size(), m = matrix2.size(), p = matrix2[0].size();
    Matrix new_matrix{algebra::zeros(n, p)};

    for (int i = 0; i < n; i ++) {
        for (int j = 0;j < p; j ++) {
            for (int k = 0; k < m; k ++) {
                new_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return new_matrix;
}

Matrix algebra::sum(const Matrix& matrix, double c) {
    Matrix new_matrix{matrix};
    for (auto& row : new_matrix) {
        for (auto& ele : row) {
            ele += c;
        }
    }
    return new_matrix;
}

bool check_sum(const Matrix& matrix1, const Matrix& matrix2) {
    if (is_empty(matrix1) && is_empty(matrix2)) return true;
    if (matrix1.size() == matrix2.size() && matrix1[0].size() == matrix2[0].size()) return true;
    return false;
}

Matrix algebra::sum(const Matrix& matrix1, const Matrix& matrix2) {
    if (!check_sum(matrix1, matrix2)) throw std::logic_error("Logic error");
    if (matrix1.empty() && matrix2.empty()) return Matrix{};

    int n = matrix1.size(), m = matrix1[0].size();
    Matrix new_matrix{zeros(n, m)};

    for (int i = 0; i < n;i ++) {
        for (int j = 0; j < m; j ++) {
            new_matrix[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return new_matrix;
}

Matrix algebra::transpose(const Matrix& matrix) {
    if (matrix.empty()) return matrix;

    int n = matrix.size(), m = matrix[0].size();
    Matrix new_matrix{zeros(m, n)};
    for (int i = 0; i < m; i ++) {
        for (int j = 0; j < n; j ++) {
            new_matrix[i][j] = matrix[j][i];
        }
    }
    return new_matrix;
}

Matrix algebra::minor(const Matrix& matrix, size_t n, size_t m) {
    if (is_empty(matrix)) throw std::logic_error("Logic error");
    if (n < 0 || m < 0) throw std::logic_error("Logic error");

    int matrix_n = matrix.size(), matrix_m = matrix[0].size();
    if (matrix_n <= n || matrix_m <= m) throw std::logic_error("Logic error");

    Matrix new_matrix{zeros(matrix_n - 1,matrix_m - 1)};

    for (int i = 0; i < matrix_n; i ++) {
        if (i == n) continue;
        for (int j = 0; j < matrix_m; j ++) {
            if (j == m) continue;
            new_matrix[i - (i > n)][j - (j > m)] = matrix[i][j];
        }
    }
    return new_matrix;
}

void check_determinant(const Matrix& matrix) {
    if (matrix.size() != matrix[0].size()) throw std::logic_error("Logic error");
}

double do_determinant(const Matrix& matrix) {
    double sum = 0;
    for (int i = 0; i < matrix.size(); i ++) {
        if (matrix[0][i] == 0) continue;
        sum += ((i % 2) == 0 ? 1 : -1) * matrix[0][i] * algebra::determinant(algebra::minor(matrix, 0, i));
    }
    return sum;
}

double algebra::fast_determinant(const Matrix& matrix) {
    Matrix new_matrix = upper_triangular(matrix);
    double sum = 1;
    for (int i = 0; i < matrix.size(); i ++) {
        if (equal_zero(new_matrix[i][i])) return 0;
        sum *= new_matrix[i][i];
    }
    return sum;
}

double algebra::determinant(const Matrix& matrix) {
    // TODO: need to do?
    //  1. check every row and column to find a line which have the largest mount of zero
    //  2. if there is a all-zero line, determinant is 0

    if (is_empty(matrix)) return 1;
    check_determinant(matrix);
    return do_determinant(matrix);
}

Matrix algebra::inverse(const Matrix& matrix) {
    if (is_empty(matrix)) return matrix;
    double deter = determinant(matrix);
    if (deter == 0) throw std::logic_error("Logic error");

    int n = matrix.size(), m = matrix[0].size();
    Matrix new_matrix{zeros(n, m)};

    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < m; j ++) {
            new_matrix[i][j] = determinant(minor(matrix, i, j));
        }
    }

    return multiply(transpose(new_matrix), 1.0 / deter);
}

Matrix algebra::concatenate(const Matrix& matrix1, const Matrix& matrix2, int axis) {
    // invalid axis
    if (axis != 0 && axis != 1) throw std::logic_error("Logic error");
    if (is_empty(matrix1)) return matrix2;
    else if (is_empty(matrix2)) return matrix1;

    int matrix1_n = matrix1.size(), matrix1_m = matrix1[0].size();
    int matrix2_n = matrix2.size(), matrix2_m = matrix2[0].size();
    // size is not match
    if (!(axis && (matrix1_n == matrix2_n) || !axis && (matrix1_m == matrix2_m))) throw std::logic_error("Logic error");

    Matrix new_matrix{};
    if (axis) {
        new_matrix.resize(matrix1_n);
        for (int i = 0; i < matrix1_n; i ++) {
            new_matrix[i].insert(new_matrix[i].end(), matrix1[i].begin(), matrix1[i].end());
            new_matrix[i].insert(new_matrix[i].end(), matrix2[i].begin(), matrix2[i].end());
        }
    } else {
        for (int i = 0; i < matrix1_n; i ++) {
            new_matrix.push_back(matrix1[i]);
        }
        for (int i = 0; i < matrix2_n; i ++) {
            new_matrix.push_back(matrix2[i]);
        }
    }
    return new_matrix;
}



Matrix algebra::ero_swap(const Matrix& matrix, size_t r1, size_t r2) {
    // empty matrix can't swap
    // r1 and r2 must have correct range
    if (is_empty(matrix) || r1 >= matrix.size() || r2 >= matrix.size()) throw std::logic_error("Logic error");
    if (r1 == r2) return matrix;

    Matrix new_matrix{matrix};
    new_matrix[r1].swap(new_matrix[r2]);
    return new_matrix;
}

Matrix algebra::ero_multiply(const Matrix& matrix, size_t r, double c) {
    if (is_empty(matrix) || r >= matrix.size()) throw std::logic_error("Logic error");

    Matrix new_matrix{matrix};
    std::transform(new_matrix[r].begin(), new_matrix[r].end(), new_matrix[r].begin(), [c](double& ele) {
        return ele * c;
    });
    return new_matrix;
}

Matrix algebra::ero_sum(const Matrix& matrix, size_t r1, double c, size_t r2) {
    if (is_empty(matrix) || r1 >= matrix.size() || r2 >= matrix.size()) throw std::logic_error("Logic error");
    if (r1 == r2) return ero_multiply(matrix, r1, c - 1);
    if (equal_zero(c)) return matrix;

    Matrix new_matrix{matrix};
    for (int i = 0; i < new_matrix[0].size(); i ++) {
        new_matrix[r2][i] += c * new_matrix[r1][i];
    }
    return new_matrix;
}


void elimination(Matrix& matrix, int row, int col) {
    // TODO:
    // 1. get a non-zero row from up to down
    // 2. if this row is not [first row], swap two row and multiply -1 to first row
    // 3. eliminate these row which is under first row
    // 4. recursive [elimination]
    // edge: last row or last column
    if (row >= matrix.size() || col >= matrix[0].size()) return;

    for (int i = row; i < matrix.size(); i ++) {
        auto& r = matrix[i];
        if (!equal_zero(r[col])) {
            if (i != row) {
                matrix = algebra::ero_swap(matrix, i, row);
                matrix = algebra::ero_multiply(matrix, row, -1);
//                r.swap(matrix[row]);
//                std::transform(matrix[row].begin(), matrix[row].end(), matrix[row].begin(), [](double& ele) {
//                    return -ele;
//                });
            }
            break;
        }
    }
    if (equal_zero(matrix[row][col])) {
        elimination(matrix, row, col + 1);
        return;
    }

    for (int i = row + 1; i < matrix.size(); i ++) {
        double coff = -matrix[i][col] / matrix[row][col];
//        if (equal_zero(coff)) continue;
//        for (int j = col; j < matrix[0].size(); j ++) {
//            matrix[i][j] += coff * matrix[row][j];
//        }
        matrix = algebra::ero_sum(matrix, row, coff, i);
    }

    elimination(matrix, row + 1, col + 1);
}

Matrix algebra::upper_triangular(const Matrix& matrix) {
    if (is_empty(matrix)) return matrix;
    if (matrix.size() != matrix[0].size()) throw std::logic_error("Logic error");

    Matrix new_matrix{matrix};
    elimination(new_matrix, 0, 0);
    return new_matrix;
}