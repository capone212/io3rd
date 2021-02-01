#include <iostream>
#include <array>
#include <string>
#include <memory>

template<std::size_t N>
using row_t = std::array<int, N>;

template <std::size_t N >
using matrix_t = std::array<row_t<N>, N>;


// using matrix_sp_t = std::unique_ptr<matrix_t>;

template<std::size_t N>
void init_random(matrix_t<N>& matrix) {
    for (auto& row : matrix) {
        for (auto& v : row) {
            v = rand() % 100 - 50;
        }
    }
}

template<std::size_t N>
auto multiply(const matrix_t<N>& left, const matrix_t<N>& right) {
    auto result = std::make_unique<matrix_t<N>>();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            (*result)[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                (*result)[i][j] += left[i][k]*right[k][j];
            }
        }
    }
    return result;
}


template<std::size_t N>
void print(const matrix_t<N>& matrix, const std::string& name) {
    std::cout << name << "=[" << std::endl;
    for (auto& row : matrix) {
        for (auto& v : row) {
            std::cout << v << " ";
        }
        std::cout << ";" << std::endl;
    }
    std::cout << "]"  << std::endl;
}

// Optmization 1: vector instructions
// Optimization 2: mutlithreaded multiply
// Optimization 3: algorithmic


int main() {
    constexpr std::size_t N = 2048;
    auto A = std::make_unique<matrix_t<N>>();
    auto B = std::make_unique<matrix_t<N>>();
    init_random(*A);
    init_random(*B);
    multiply(*A, *B);
    // print(A, "A");
    // print(B, "B");
    // print(C, "C");
    return 0;
}