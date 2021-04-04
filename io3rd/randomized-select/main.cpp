#include <iostream>
#include <span>
#include <algorithm>
#include <vector>
#include <random>
#include <cassert>

int partition(std::span<int> span) {
    if (span.empty()) {
        throw std::runtime_error("invalid input");
    }
    auto x = span.back();
    int k = 0;
    // move all elements that are less than to beginig
    for (int& i : span.first(span.size()-1)) {
        if (i <= x) {
            std::swap(i, span[k++]);
        }
    }
    std::swap(span[k], span.back());
    return k;
}

// param: order=[0, n-1]
// param: complexity is for debug and should be removed
int rand_select(std::span<int> span, int order, std::int64_t& complexity) {
    complexity += span.size();

    if (span.size() == 1) {
        assert(order == 0);
        return span.front();
    }
    int pivot = partition(span);
    if (pivot == order) {
        return span[pivot];
    }
    if (pivot > order) {
        return rand_select(span.first(pivot), order, complexity);
    } else {
        return rand_select(span.last(span.size() - pivot - 1), order - pivot - 1, complexity);
    }
}



int main() {
    srand (time(NULL));
    std::cout << "Init" <<  std::endl;
    std::vector<int> buffer;
    buffer.resize(800*1024*1024);
    for (int& i : buffer) {
        i = rand();
    }
    std::cout << "Starting calc..." <<  std::endl;
    int order = 2695;
    std::int64_t complexity = 0;
    int result_linear = rand_select(buffer, order, complexity);
    std::cout << "Result linear: " << result_linear << " complexity:" << double(complexity/1024/1024)/(buffer.size()/1024/1024) <<  std::endl;
    return 0;
}