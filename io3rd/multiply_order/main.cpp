#include <iostream>
#include <span>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm>
#include <unordered_map>

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
    if (input.size() == 1) {
        return Result{.dim = input.front()};
    }
    if (input.size() == 2) {
        return multiply(input.front(), input.back());
    }
    Result result {.cost = std::numeric_limits<std::int64_t>::max()};
    for (int pivot = 1; pivot < input.size(); ++pivot)  {
        auto head = min_multiply_count_brute_force(input.first(pivot));
        auto tail = min_multiply_count_brute_force(input.last(input.size() - pivot));
        auto product = multiply(head, tail);
        if (product.cost < result.cost) {
            result = product;
        }
    }
    return result;
}

using processed_costs_t = std::map<dimensions_list_t, Result>;

Result min_multiply_count_dp_top_down(std::span<dimensions_t> input, processed_costs_t& memo) {
    if (input.size() == 1) {
        return Result{.dim = input.front()};
    }
    if (input.size() == 2) {
        return multiply(input.front(), input.back());
    }
    if (auto it = memo.find(input); it != memo.end()) {
        return it->second;
    }
    Result result {.cost = std::numeric_limits<std::int64_t>::max()};
    for (int pivot = 1; pivot < input.size(); ++pivot)  {
        auto head = min_multiply_count_dp_top_down(input.first(pivot), memo);
        auto tail = min_multiply_count_dp_top_down(input.last(input.size() - pivot), memo);
        auto product = multiply(head, tail);
        if (product.cost < result.cost) {
            result = product;
        }
    }
    memo[input] = result;
    return result;
}

Result min_multiply_count_dp_botom_up(std::span<dimensions_t> input) {
    using index_t = std::pair<int /*offset*/, int /*length*/>;
    std::map<index_t, Result> memo;
    // init table
    for (int i = 0; i < input.size(); ++i) {
        memo[std::make_pair(i,1)] = Result{.dim = input[i]};
    }
    for (int length = 2; length <= input.size(); ++length) {
        for (int offset = 0; offset + length <= input.size(); ++offset) {
            Result result {.cost = std::numeric_limits<std::int64_t>::max()};
            for (int k = 1; k < length; ++k) {
                auto head = memo[std::make_pair(offset, k)];
                auto tail = memo[std::make_pair(offset+k, length - k)];
                auto product = multiply(head, tail);
                if (product.cost < result.cost) {
                    result = product;
                }
            }
            memo[std::make_pair(offset,length)] = result;
        }
    }
    return memo[std::make_pair(0,input.size())];
}

int main() {
    // std::vector<dimensions_t> operations =  {
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     {10,100}, {100,5}, {5, 50}, {50, 2500}, {2500,3}, {3, 10}, {10, 1000}, {1000,1}, {1, 25}, {25,10},
    //     };
    std::vector<dimensions_t> operations =  {
        {30,35}, {35,15}, {15, 5}, {5, 10}, {10,20}, {20, 25},
    };
    {
        auto result = min_multiply_count_brute_force(operations);
        std::cout << "brute force cost:" << result.cost << std::endl
            << "dims row:" << result.dim.first << " col:" <<  result.dim.second << std::endl << std::endl;
    }
    {
        processed_costs_t memo;
        auto result = min_multiply_count_dp_top_down(operations, memo);
        std::cout << "top down cost:" << result.cost << std::endl
            << "dims row:" << result.dim.first << " col:" <<  result.dim.second << std::endl << std::endl;
    }
    {
        auto result = min_multiply_count_dp_botom_up(operations);
        std::cout << "botom up cost:" << result.cost << std::endl
            << "dims row:" << result.dim.first << " col:" <<  result.dim.second << std::endl << std::endl;
    }
    return 0;
}