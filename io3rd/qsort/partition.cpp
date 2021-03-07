#include <iostream>
#include <span>
#include <vector>

void print(const auto& list) {
    for (const auto& l : list) {
        std::cout << l << "," ;
    }
    std::cout << std::endl;
}

int partition(const std::span<int>& span) {
    if (span.empty()) {
        throw std::runtime_error("invalid input");
    }
    auto x = span.back();
    int k1 = 0;
    // move all elements that are less than to beginig
    for (int& i : span.first(span.size()-1)) {
        if (i < x) {
            std::swap(i, span[k1++]);
        }
    }
    int k2 = k1;
    for (int& i : span.last(span.size()-k1)) {
        if (i <= x) {
            std::swap(i, span[k2++]);
        }
    }
    std::cout << "k1=" << k1 << " k2=" << k2 << std::endl;
    return (k1 + k2) / 2;
}

int main() {
    // std::vector<int> buffer = {6,8,9,5,2,4,7};
    std::vector<int> buffer = {7};
    int q = partition(buffer);
    std::cout << "partitioned over pivot:" << buffer[q] << " with index:" << q << std::endl;
    print(buffer);
    return 0;
}