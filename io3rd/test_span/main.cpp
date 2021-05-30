#include <iostream>
#include <span>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using operations_t = std::span<int>;

namespace std {
    constexpr auto operator<=>(operations_t lhs, const operations_t rhs) {
        return std::lexicographical_compare_three_way(begin(lhs), end(lhs), begin(rhs), end(rhs));
    }
}
  

int main() {
    std::vector<int> buff = {1, 2, 3, 4, 5, 6, 7};
    using hash_table_t = std::map<operations_t, std::string>;
    hash_table_t table;
    table[buff] = "hello";
    return 0;
}
