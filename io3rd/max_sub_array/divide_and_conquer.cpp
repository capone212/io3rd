#include <iostream>
#include <vector>

template<typename TList>
void printVector(const TList& list) {
    for (auto v : list) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

using intlist_t = std::vector<int>;

struct TestCase {
    intlist_t input;
    int result;
};


intlist_t transform(const intlist_t& input) {
    intlist_t result;
    if (input.empty()) {
        return result;
    }
    auto last = input.front();
    for (auto val : input) {
        result.push_back(val - last);
        last = val;
    }
    return result;
}

int max_sub_array_from_point_to_direction(const intlist_t& input, int index, int step, int low, int hight) {
    int current_sum = 0;
    int max_sum = 0; 
    while(index >= low && index <= hight) {
        current_sum += input.at(index);
        if (current_sum > max_sum) {
            max_sum = current_sum;
        }
        index += step;
    }
    return max_sum;
}

int max_sub_array_for_point(const intlist_t& input, int index, int low, int hight) {
    int left_sum = max_sub_array_from_point_to_direction(input, index-1, -1, low, hight);
    int right_sum = max_sub_array_from_point_to_direction(input, index+1, 1, low, hight);
    return input.at(index) + left_sum + right_sum;
}

int max_sub_array(const intlist_t& input, int low, int hight) {
    if (low == hight) {
        return input.at(low);
    }
    // low  hight   mid  left_range  right_range
    // 0    1       0    [0,0]       [1, 1]
    int mid = (low + hight) / 2;
    int left_subaray = max_sub_array(input, low, mid);
    int right_subaray = max_sub_array(input, mid+1, hight);
    int sum_midpoint = max_sub_array_for_point(input, mid, low, hight);
    std::cout << "max_sub_array low:" << low << " hight:" << hight << " midle:" << mid << " lsum:" << left_subaray << " rsum:" 
        << right_subaray << " midsum:" << sum_midpoint  << std::endl;
    int max_subs = std::max(left_subaray, right_subaray);
    return std::max(sum_midpoint, max_subs);
}

int max_sub_array(const intlist_t& input) {
    if (input.empty()) {
        return 0;
    }
    intlist_t list = transform(input);
    printVector(list);
    return max_sub_array(list, 0, list.size() -1);
}

int main() {    
    std::vector<TestCase> tests = {
        {{100, 113, 110, 85, 105, 102, 86, 63, 81, 101, 94, 106, 101, 79, 94, 90, 97}, 43}
    };
    for (const auto& v : tests) {
        auto summ = max_sub_array(v.input);
        if  (summ != v.result) {
            std::cout << "Test case failed. Expected " << v.result << " but got:" << summ << std::endl;
            return 1;
        } 
    }
    std::cout << "ok" << std::endl;
    return 0;
}