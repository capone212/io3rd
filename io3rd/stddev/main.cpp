#include <iostream>
#include <map>

struct RVInfo {
    int counter = 0; 
};

double expected_value(auto rv_func) {
    std::map<int, RVInfo> rvariables;
    for (int i = 1; i <=6; ++i) {
        for (int j = 1; j <=6; ++j) {
            int rv = rv_func(i, j);
            ++rvariables[rv].counter;
        }
    }
    double result = 0;
    for (const auto& [rv, info]: rvariables) {
        result += double(rv * info.counter)/36;
    }
    return result;
}

int main() {
    std::cout <<"Hello world!" << std::endl;
    std::cout <<"Expected value summ: " <<  expected_value([](int i, int j) {
        return i + j;
    }) << std::endl;

    std::cout <<"Expected value max: " <<  expected_value([](int i, int j) {
        return std::max(i, j);
    }) << std::endl;
    return 0;
}