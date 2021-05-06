#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "solution.h"


struct TestCase
{
    TestCase(const std::vector<int>& input_, int res) :
        input(input_),
        result(res)
    {

    }

    std::vector<int> input;
    int result = 0;
};

std::ostream& operator<<(std::ostream& stream, const TestCase& t)
{
    // TODO:
    stream << "input:" << "" << " expected:" << t.result;
    return stream;
}

int main()
{
    std::vector<TestCase> testCases = {
        TestCase{{0,1,0,2,1,0,1,3,2,1,2,1}, 6},
        TestCase{{4,2,0,3,2,5}, 9},
        TestCase{{}, 0},
        TestCase{{1}, 0},
        TestCase{{1,0}, 0},
    };

    for (const auto& t : testCases)
    {
        Solution s;
        int longest =  s.trap(t.input);
        if (longest != t.result) {
            std::cout << "Test case failed: " << t 
                << " result:" << longest << std::endl;
            return 1;
        }
        // std::cout << "Counter:" << s.getCounter() << std::endl;
    }
    std::cout << "Ok" << std::endl;
    return 0;
}