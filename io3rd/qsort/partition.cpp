#include <iostream>
#include <span>
#include <algorithm>
#include <vector>
#include <random>

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

inline void insertion_sort(std::span<int> span) {
    int key, j;
    for(int i = 1; i<span.size(); i++) {
        key = span[i];//take value
        j = i;
        while(j > 0 && span[j-1]>key) {
            span[j] = span[j-1];
            j--;
        }
        span[j] = key;   //insert in right place
    }
}


void qsort(std::span<int> span) {
    if (span.size() < 8) {
        insertion_sort(span);
        return;
    }
    int pivot = partition(span);
    qsort(span.first(pivot));
    qsort(span.last(span.size() - pivot - 1));
}



int main() {
    std::vector<int> buffer;
    buffer.resize(100*1024*1024);
    for (int& i : buffer) {
        i = rand();
    }
    std::cout << "Sorted started..." << std::endl;
    // std::sort(buffer.begin(), buffer.end());
    qsort(buffer);
    std::cout << "Sorted: " << std::is_sorted(buffer.begin(), buffer.end()) << std::endl;
    return 0;
}