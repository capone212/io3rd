#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <string.h>
#include <assert.h>

int MaxPrefix(const std::string& s, int position, int startFrom = 0) {
    int z = startFrom;
    while (position + z < s.size() && s[position + z] == s[z]) {
        ++z;
    }
    return z;
}

std::vector<int> ZBruteForce(const std::string& s)
{
    std::vector<int> result(s.size(), 0);
    for (int i = 1; i < s.size(); ++i) {
        result[i] = MaxPrefix(s, i);
    }
    return result;
}

std::vector<int> ZLinear(const std::string& s)
{
    std::vector<int> zvalue(s.size(), 0);
    int l = 0;
    int r = 0;
    for (int i = 1; i < s.size(); ++i) {
        if (i >= r) {
            l = i;
            zvalue[i] = MaxPrefix(s, i);
            r = i + zvalue[i];
        } else {
            int deltaL = i - l;
            int deltaR = r - i;
            assert(deltaL > 0);
            assert(deltaR > 0);

            if (zvalue[deltaL] < deltaR) {
                zvalue[i] = zvalue[deltaL];
            } else {
                l = i;
                zvalue[i] = MaxPrefix(s, i, deltaR);
                r = i + zvalue[i];
            }
        }
    }
    return zvalue;
}

std::string Stringify(const std::vector<int>& v)
{
    std::stringstream ss;
    for (size_t i = 0; i < v.size(); i++) {
        if (i != 0) {
            ss << ",";
        }
        ss << v[i];
    }
    return ss.str();
}

struct TTestCase {
    std::string text;
    std::vector<int> result;
};

std::string GetRandomString(const int len, int characters) {
    static const char ALPHANUM[] = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(len);

    characters = std::max<int>(characters, 1);
    characters = std::min<int>(characters, sizeof(ALPHANUM) - 1);

    for (int i = 0; i < len; ++i) {
        result += ALPHANUM[rand() % characters];
    }

    return result;
}

bool CheckTest(const TTestCase& t) {
    const auto result = ZLinear(t.text);
    if (result != t.result) {
        std::cout << "Test case: " << t.text << " failed!"
            << " expected: '" << Stringify(t.result)
            << "' got: '" << Stringify(result) << "'"
            << std::endl;
        return false;
    }
    return true;
}

int main()
{
    srand((unsigned)time(NULL)); 

    std::vector<TTestCase> testCases = {
        {
            .text = "aabcaabxaaz",
            .result = {0,1,0,0,3,1,0,0,2,1,0},
        },
        {
            .text = "a",
            .result = {0},
        },
        {
            .text = "aaaaaaaaaaaa",
            .result = {0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
        },
        {
            .text = "",
            .result = {}
        },
    };

    for (const auto& t : testCases) {
        if (!CheckTest(t)) {
            return 1;
        }
    }

    for (int i = 0; i < 1000000; ++i) {
        TTestCase t;
        t.text = GetRandomString(1000, 10);
        t.result = ZBruteForce(t.text);

        const auto result = ZLinear(t.text);

        if (!CheckTest(t)) {
            return 1;
        }
    }


    std::cout << "OK" << std::endl;

    return 0;
}
