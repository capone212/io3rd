#include <iostream>
#include <span>
#include <algorithm>
#include <vector>
#include <random>
#include <ranges>

const int MAX_DIGIT = 32*1024;

void counting_sort(std::span<int> span) {
    std::vector<int> stats(MAX_DIGIT, 0);
    for (int i : span) {
        ++stats[i];
    }
    for (int i = 1; i < stats.size(); ++i) {
        stats[i]+=stats[i-1];
    }
    int index = 0;
    for (int k = 0; k < stats.size(); ++k) {
        for (int i = index; i < stats[k]; ++i) {
            span[index++] = k;
        }
    }
}

std::vector<int> counting_sort_stable(std::span<int> span) {
    std::vector<int> stats(MAX_DIGIT, 0);
    std::vector<int> result(span.size(), 0);
    stats.resize(MAX_DIGIT, 0);
    for (int i : span) {
        ++stats[i];
    }
    for (int i = 1; i < stats.size(); ++i) {
        stats[i]+=stats[i-1];
    }

    for (int x : std::views::reverse(span)) {
        result[stats[x]] = x;
        --stats[x];
    }
    return result;
}

void print(const auto& list) {
    for (const auto& l : list) {
        std::cout << l << "," ;
    }
    std::cout << std::endl;
}

int main() {
    // std::vector<int> buffer;
    // buffer.resize(1024*1024*1024);
    // for (int& i : buffer) {
    //     i = rand() % MAX_DIGIT;
    // }
    std::vector<int> buffer = {2,3,5,3,2,1,46,7};
    std::cout << "Sorted started..." << std::endl;
    // counting_sort(buffer);
    auto result = counting_sort_stable(buffer);
    print(result);
    std::cout << "Sorted: " << std::is_sorted(result.begin(), result.end()) << std::endl;
    return 0;
}