#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <span>

template<std::size_t N>
using row_t = std::array<int, N>;
template <std::size_t N >
using matrix_t = std::array<row_t<N>, N>;

// static_assert(std::tuple_size<matrix_t<10>>::value == 10);

template<std::size_t N>
using row_view_t = std::span<int, N>;
// template<std::size_t N>
// using matrix_view_t = std::span<row_view_t<N>, N>;


template<std::size_t N> 
using sub_matrix_t = std::array<row_view_t<N>, N>;

template<std::size_t N>
void init_random(matrix_t<N>& matrix) {
    for (auto& row : matrix) {
        for (auto& v : row) {
            v = rand() % 100 - 50;
        }
    }
}

template<std::size_t N>
void multiply_quibic_view(const sub_matrix_t<N>& left, const sub_matrix_t<N>& right, sub_matrix_t<N> result) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                result[i][j] += left[i][k]*right[k][j];
            }
        }
    }
}

template<std::size_t N>
using sub_matrix_ps =  std::unique_ptr<sub_matrix_t<N>>;

template<std::size_t N>
sub_matrix_ps<N> make_view(matrix_t<N>& matrix)
{
    return [&] <std::size_t... Is> (std::index_sequence<Is...>){ 
        return sub_matrix_ps<N>{ new sub_matrix_t<N>{matrix[Is]...} }; 
    }(std::make_index_sequence<N>{});
}

template<std::size_t N>
auto multiply_log7(const matrix_t<N>& left, const matrix_t<N>& right) {
    constexpr const std::size_t halthN = N/2;

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

// template <typename T, size_t N, size_t... I>
// auto subarray_impl(std::array<T, N> const& arr,
//                    size_t first,
//                    std::index_sequence<I...>)
//     -> std::array<T, sizeof...(I)>
// {
//     return {arr[first + I]...};
// }


int main() {
    constexpr std::size_t N = 2048;
    auto A = std::make_unique<matrix_t<N>>();
    auto B = std::make_unique<matrix_t<N>>();
    auto C = std::make_unique<matrix_t<N>>();
    init_random(*A);
    init_random(*B);
    auto Aref = make_view(*A);
    auto Bref = make_view(*B);
    auto Cref = make_view(*C);

    multiply_quibic_view(*Aref, *Bref, *Cref);
    // multiply_quibic_view(make_view(*A), make_view(*B), make_view(*C));
    // // print(A, "A");
    // // print(B, "B");
    // // print(C, "C");
    return 0;
}