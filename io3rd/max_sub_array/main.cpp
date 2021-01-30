#include <iostream>
#include <vector>

using intlist_t = std::vector<int>;

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

template<typename TList>
void printVector(const TList& list) {
    for (auto v : list) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

int max_sub_array(const intlist_t& input) {
    intlist_t list = transform(input);
    int max_sum = 0;
    int last_sum = 0;

    for (auto delta : list) {
        if (last_sum < 0) {
            last_sum = delta;
        } else {
            last_sum += delta;
        }

        if (last_sum > max_sum) {
            max_sum = last_sum;
        }
    }
    return max_sum;
}

struct TestCase {
    intlist_t input;
    int result;
};   

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