#include <iostream>
#include <span>
#include <algorithm>
#include <vector>
#include <random>
#include <ranges>
#include <cassert>

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

// Get n-th byte from integer
inline int nth_byte(int number, int n) {
    return (number >> (8*n)) & 0xff;
}

void radix_sort(std::span<int> span) {
    using buffer_t = std::vector<int>;
    std::vector<buffer_t> rstore;
    rstore.resize(256);
    for (int d = 0; d < 4; ++d) {
        // Map to location
        for (int i : span) {
            rstore[nth_byte(i, d)].push_back(i);
        }
        // Copy back in order
        int index = 0;
        for (auto& batch : rstore) {
            for (int x : batch) {
                span[index++] = x;
            }
            batch.clear();
        }
        assert(index == span.size());
    } 
}

void print(const auto& list) {
    for (const auto& l : list) {
        std::cout << l << "," ;
    }
    std::cout << std::endl;
}

int main() {

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0);

    std::vector<int> buffer;
    buffer.resize(1024*1024*1024);
    for (int& i : buffer) {
        i = distrib(gen);
    }
    // std::vector<int> buffer = {32343441, 2,3,5,3,2,1,46,7, 256, 1024, 32343434};
    std::cout << "Sorted started..." << std::endl;
    // counting_sort(buffer);
    radix_sort(buffer);
    // print(buffer);
    std::cout << "Sorted: " << std::is_sorted(buffer.begin(), buffer.end()) << std::endl;
    return 0;
}