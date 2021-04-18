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
int rand_select(std::span<int> span, int order) {
    if (span.size() == 1) {
        assert(order == 0);
        return span.front();
    }
    int pivot = partition(span);
    if (pivot == order) {
        return span[pivot];
    }
    if (pivot > order) {
        return rand_select(span.first(pivot), order);
    } else {
        return rand_select(span.last(span.size() - pivot - 1), order - pivot - 1);
    }
}

int rand_select_non_recursive(std::span<int> span, int order) {
    while (true) {
        if (span.size() == 1) {
            return span.front();
        }
        int pivot = partition(span);
        if (pivot == order) {
            return span[pivot];
        }
        if (pivot > order) {
            span = span.first(pivot);
        } else {
            span = span.last(span.size() - pivot - 1);
            order -= pivot + 1;
        }
    }
}

int main() {
    srand (time(NULL));
    std::cout << "Init" <<  std::endl;
    std::vector<int> buffer;
    buffer.resize(100*1024*1024);
    for (int& i : buffer) {
        i = rand();
    }
    std::cout << "Starting calc..." <<  std::endl;
    int order = 50*1024*1024;
    std::cout << "Result recursive: " << rand_select(buffer, order) << std::endl;
    // std::cout << "Result non-recursive: " << rand_select_non_recursive(buffer, order) << std::endl;
    return 0;
}