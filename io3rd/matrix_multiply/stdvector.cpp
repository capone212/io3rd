#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <memory>


using row_t = std::vector<int>;
using matrix_t = std::vector<row_t>;


// using matrix_sp_t = std::unique_ptr<matrix_t>;

void init_random(matrix_t& matrix, int size) {
    matrix.resize(size);
    for (auto& row : matrix) {
        row.resize(size);
        for (auto& v : row) {
            v = rand() % 100 - 50;
        }
    }
}

void init_zero(matrix_t& matrix, int size) {
    matrix.resize(size);
    for (auto& row : matrix) {
        row.resize(size);
        for (auto& v : row) {
            v = 0;
        }
    }
}

auto multiply(const matrix_t& left, const matrix_t& right) {
    matrix_t result;
    init_zero(result, left.size());
    for (int i = 0; i < result.size(); ++i) {
        for (int j = 0; j < result.size(); ++j) {
            for (int k = 0; k < result.size(); ++k) {
                result[i][j] += left[i][k]*right[k][j];
            }
        }
    }
    return result;
}

int main() {
    constexpr std::size_t N = 2048;
    matrix_t A;
    matrix_t B;
    init_random(A, N);
    init_random(B, N);
    multiply(A, B);
    return 0;
}