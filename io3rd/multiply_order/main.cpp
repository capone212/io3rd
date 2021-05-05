#include <iostream>
#include <span>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm>

using dimensions_t =  std::pair<int, int>;
using dimensions_list_t = std::span<dimensions_t>;
namespace std {
    constexpr auto operator<=>(dimensions_list_t lhs, const dimensions_list_t rhs) {
        return std::lexicographical_compare_three_way(begin(lhs), end(lhs), begin(rhs), end(rhs));
    }
}
struct Result {
    dimensions_t dim;
    std::int64_t cost = 0;
};

using processed_costs_t = std::map<dimensions_list_t, Result>;


Result multiply(dimensions_t left, dimensions_t right, int64_t extra_cost = 0) {
    assert(left.second == right.first);
    return Result {
        .dim = std::make_pair(left.first, right.second),
        .cost = left.first * left.second * right.second + extra_cost
    };
}

Result multiply(const Result& l, const Result& r) {
    return multiply(l.dim, r.dim, l.cost + r.cost);
}

Result min_multiply_count_brute_force(std::span<dimensions_t> input) {
    if (input.size() == 2) {
        return multiply(input.front(), input.back());
    }
    auto left = multiply(Result{.dim = input.front()}, min_multiply_count_brute_force(input.last(input.size()-1)));
    auto right = multiply(min_multiply_count_brute_force(input.first(input.size()-1)), Result{.dim = input.back()});
    return (left.cost < right.cost) ? left : right;
}

using processed_costs_t = std::map<dimensions_list_t, Result>;

Result min_multiply_count_dp(std::span<dimensions_t> input, processed_costs_t& memo) {
    if (input.size() == 2) {
        return multiply(input.front(), input.back());
    }
    if (auto it = memo.find(input); it != memo.end()) {
        return it->second;
    }
    auto left = multiply(Result{.dim = input.front()}, min_multiply_count_dp(input.last(input.size()-1), memo));
    auto right = multiply(min_multiply_count_dp(input.first(input.size()-1), memo), Result{.dim = input.back()});
    auto result = (left.cost < right.cost) ? left : right;
    memo[input] = result;
    return result;
}

int main() {
    std::vector<dimensions_t> operations =  {
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
        };
    processed_costs_t memo;
    auto result = min_multiply_count_dp(operations, memo);
    std::cout << "cost:" << result.cost << std::endl
        << "dims row:" << result.dim.first << " col:" <<  result.dim.second << std::endl;
    return 0;
}