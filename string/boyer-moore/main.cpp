#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <string_view>

namespace std {
    template<typename TCont>
    int ssize(const TCont& cont) {
        return ::std::size(cont);
    }
}

int MaxPrefix(const std::string& s, int position, int startFrom = 0) {
    int z = startFrom;
    while (position + z < s.size() && s[position + z] == s[z]) {
        ++z;
    }
    return z;
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

std::vector<int> LDashLinear(std::string s) {
    std::reverse(s.begin(), s.end());
    auto zreverse = ZLinear(s);
    std::reverse(zreverse.begin(), zreverse.end());

    std::vector<int> result(zreverse.size(), 0);
    int n = std::ssize(s) - 1;
    for (int j = 1; j < n; ++j) {
        int i = n - zreverse[j] + 1;
        if (i > n) {
            continue;
        }
        result[i] = j;
    }

    return result;
}

int FindLMatch(const std::string& s, int startFrom) {
    const auto pattern = std::string_view(s).substr(startFrom);

    for (int i = std::ssize(s) - 2; i > 0; --i) {
        auto substring = std::string_view(s).substr(0, i + 1);
        if (substring.ends_with(pattern)) {
            return i;
        }
    }

    return 0;
}

std::vector<int> LBruteForce(const std::string& s)
{
    std::vector<int> result(s.size(), 0);
    for (int i = 1; i < s.size(); ++i) {
        result[i] = FindLMatch(s, i);
    }
    return result;
}

int FindLDashMatch(const std::string& s, int startFrom) {
    const auto pattern = std::string_view(s).substr(startFrom);
    char mismatchChar = s[startFrom - 1];

    for (int i = std::ssize(s) - 2; i > 0; --i) {
        auto substring = std::string_view(s).substr(0, i + 1);

        if (!substring.ends_with(pattern)) {
            continue;
        }

        int charBeforeIndex = i - std::ssize(pattern);

        if (charBeforeIndex < 0 || s[charBeforeIndex] != mismatchChar) {
            return i;
        }
    }

    return 0;
}

std::vector<int> LDashBruteForce(const std::string& s)
{
    std::vector<int> result(s.size(), 0);
    for (int i = 1; i < s.size(); ++i) {
        result[i] = FindLDashMatch(s, i);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////

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

bool CheckTest(const TTestCase& t) {
    const auto result = LDashLinear(t.text);
    if (result != t.result) {
        std::cout << "Test case: " << t.text << " failed!"
            << " expected: '" << Stringify(t.result)
            << "' got: '" << Stringify(result) << "'"
            << std::endl;
        return false;
    }
    return true;
}

std::string GetRandomString(int len, int characters) {
    static const char ALPHANUM[] = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(len);

    characters = std::max<int>(characters, 1);
    characters = std::min<int>(characters, sizeof(ALPHANUM) - 1);

    for (int i = 0; i < len; ++i) {
        result.push_back(ALPHANUM[rand() % characters]);
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////

int main()
{
    std::vector<TTestCase> testCases = {
        {
            .text = "aaaaaa",
            .result = {0, 4, 3, 2, 1, 0},
        },
        {
            .text = "zabaab",
            .result = {0, 0, 0, 0, 2, 0},
        },
        {
            .text = "abbbcbbb",
            .result = {0, 0, 0, 0, 0, 3, 6, 5},
        },
    };

    for (const auto& t : testCases) {
        if (!CheckTest(t)) {
            return 1;
        }
    }

    // Stress test
    for (int i = 0; i < 10000; ++i) {
        TTestCase t;
        t.text = GetRandomString(100, 10);
        t.result = LDashBruteForce(t.text);

        if (!CheckTest(t)) {
            return 1;
        }
    }

    // Perf test
    auto longText = GetRandomString(1'000'000, 1);
    LDashLinear(longText);

    std::cout << "OK" << std::endl;
    return 0;
}