#include <iostream>
#include <array>
#include <vector>

constexpr std::array<int, 10> PRICES = {1, 5, 8, 9, 10, 17, 17, 20, 24, 30};

int cut_rode_top_down_memo(std::vector<int>& buffer, int n) {
    if (n == 0) {
        return 0;
    }
    if (auto x = buffer[n]; x != 0) {
        return x;
    }
    int q = 0;
    const int prices_length = std::min(n, int(PRICES.size()));
    for (int i = 0; i < prices_length; ++i) {
        q = std::max(q, PRICES[i] + cut_rode_top_down_memo(buffer, n - i - 1));
    }
    buffer[n] = q;
    return q;
}

int cut_rode_bottom_up(std::vector<int>& buffer, int n) {
    for (int j = 1; j <= n; ++j) {
        const int prices_length = std::min(j, int(PRICES.size()));
        int q = 0;
        for (int i = 1; i < prices_length; ++i) {
            q = std::max(q, PRICES[i] + buffer[j - i - 1]);
        }
        buffer[j] = q;
    }
    return buffer[n];
}

int main() {
    const int size = 10'000'000;
    std::vector<int> buffer;
    buffer.resize(size + 1, 0);
    std::cout << cut_rode_bottom_up(buffer, size) << std::endl;
}