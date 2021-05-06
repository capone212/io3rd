#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <vector> 

auto remove_back(std::string_view tmp) {
    return tmp.substr(0, tmp.size()-1);
}

std::string LCS_recursive(std::string_view left, std::string_view right) {
    if (left.empty() || right.empty()) {
        return std::string();
    }
    if (left.back() == right.back()) {
        return LCS_recursive(remove_back(left), remove_back(right)) + left.back();
    }
    auto left_moved = LCS_recursive(remove_back(left), right);
    auto right_moved = LCS_recursive(left, remove_back(right));
    auto max = left_moved.size() > right_moved.size() ? left_moved : right_moved;
    return max;
}

using index_t = std::pair<int, int>;
using memo_t = std::map<index_t, std::string>;

std::string LCS_dp_top_down(std::string_view left, std::string_view right, memo_t& memo) {
    if (left.empty() || right.empty()) {
        return std::string();
    }
    auto currentIndex = std::make_pair(left.size(), right.size());
    if (auto it = memo.find(currentIndex); it != memo.end()) {
        return it->second;
    }
    if (left.back() == right.back()) {
        return LCS_dp_top_down(remove_back(left), remove_back(right), memo) + left.back();
    }
    auto left_moved = LCS_dp_top_down(remove_back(left), right, memo);
    auto right_moved = LCS_dp_top_down(left, remove_back(right), memo);
    auto max = left_moved.size() > right_moved.size() ? left_moved : right_moved;
    memo[std::make_pair(left.size(), right.size())] = max;
    return max;
}

using prefixes_t = std::vector<std::string>;
using flat_memo_t =  std::vector<prefixes_t>;

std::string get_sub_max(int max_row, int max_column, const flat_memo_t& memo) {
    std::string max;
    for (int i = 0; i < max_row; ++i) {
        for (int j = 0; j < max_column; ++j) {
            const auto& underdog = memo[i][j];
            if (underdog.size() > max.size()) {
                max = underdog;
            }
        }
    }
    return max;
}

std::string LCS_dp_bottom_up(std::string_view left, std::string_view right) {
    flat_memo_t memo;
    // init table
    {
        memo.resize(left.size());
        for (int i = 0; i < left.size(); ++i) {
            memo[i].resize(right.size());
        }
    }
    for (int i = 0; i < left.size(); ++i) {
        for (int j = 0; j < right.size(); ++j) {
            auto lcs = get_sub_max(i, j, memo);
            if (left[i] == right[j]) {
                lcs += left[i];
            }
            memo[i][j] = lcs; 
        }
    }
    std::string max;
    const int last = left.size() - 1;
    for (int j = 0; j < right.size(); ++j) {
        const auto& underdog = memo[last][j];
        if (underdog.size() > max.size()) {
            max = underdog;
        }
    }
    return max;
}

int main() {
    auto s1 = "ACCGGTCGAGTGCGCGGAAGCCGGCCGAA";
    auto s2 = "GTCGTTCGGAATGCCGTTGCTCTGTAAA";
    {
        memo_t memo;
        auto result = LCS_dp_top_down(s1, s2, memo);
        std::cout << "Recursive result:" << result << std::endl;
    }
    {
        auto result = LCS_dp_bottom_up(s1, s2);
        std::cout << "Bottom up result:" << result << std::endl;
    } 
    return 0;
}