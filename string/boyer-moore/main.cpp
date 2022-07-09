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

int FindSuffixThatIsAlsoPrefix(const std::string& s, int position)
{
    std::string_view view{s};

    for (int i = position; i < s.size(); ++i) {
        auto suffix = view.substr(i);
        if (view.starts_with(suffix)) {
            return suffix.size();
        }
    }

    return 0;
}


// LSmallDash[i] largest suffix of P[i..n] that is also a prefix of P.
std::vector<int> LSmallDashBruteForce(const std::string& s)
{
    std::vector<int> result;

    for (int i = 0; i < std::ssize(s); ++i) {
        result.push_back(FindSuffixThatIsAlsoPrefix(s, i));
    }

    return result;
}

// LSmallDash[i] equals to largest j <= n-i such that, np[j] == j.

std::vector<int> LSmallDashLinear(std::string s) {
    std::reverse(s.begin(), s.end());
    auto zreverse = ZLinear(s);
    std::reverse(zreverse.begin(), zreverse.end());

    int n = std::ssize(s);
    int lastResult = 0;
    std::vector<int> jarray(s.size(), 0);

    for (int j = 0; j < s.size(); ++j) {
        if (zreverse[j] == j + 1) {
            lastResult = j + 1;
        }
        jarray[j] = lastResult;
    }
    std::vector<int> result(zreverse.size(), 0);

    result[0] = 0;
    for (int i = 1; i < n; ++i) {
        result[i] = jarray[n-i-1];
    }

    return result;
}

bool CheckLSmalTest(const TTestCase& t) {
    const auto result = LSmallDashLinear(t.text);
    if (result != t.result) {
        std::cout << "Test case: " << t.text << " failed!"
            << " expected: '" << Stringify(t.result)
            << "' got: '" << Stringify(result) << "'"
            << std::endl;
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////

int CheckLDash()
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

    return 0;
}

int CheckLSmalDash() {
    std::vector<TTestCase> testCases = {
        // {
        //     .text = "aa",
        //     .result = {2, 1},
        // },
        // {
        //     .text = "aaaaaa",
        //     .result = {6, 5, 4, 3, 2, 1},
        // },
        // {
        //     .text = "zabaab",
        //     .result = {6, 0, 0, 0, 0, 0},
        // },
        // {
        //     .text = "bbbbcbbb",
        //     .result = {8, 3, 3, 3, 3, 3, 2, 1},
        // },
        {
            .text = "addb",
            .result = {0, 0, 0, 0},
        },
    };

    for (const auto& t : testCases) {
        if (!CheckLSmalTest(t)) {
            return 1;
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////

std::vector<int> FindAllOccurrencesBruteForce(const std::string& text, const std::string& pattern) {
    std::vector<int> positions;

    size_t pos = text.find(pattern, 0);
    while(pos != std::string::npos)
    {
        positions.push_back(pos);
        pos = text.find(pattern, pos + 1);
    }

    return positions;
}

struct TRightValues
{
    std::vector<int> Values;
    int Position = -1;

    void Push(int pos) {
        Values.push_back(pos);
        Position = Values.size();
    }

    int Get(int pos) {
        while (Position >= 0 && Values[Position] >= pos) {
            --Position;
        }

        if (Position >= 0) {
            return Values[Position];
        }

        return pos;
    }

};

std::unordered_map<char, TRightValues> RightValues(const std::string& text) {
    std::unordered_map<char, TRightValues> result;

    for (int i = 0; i < std::ssize(text); ++i) {
        result[text[i]].Push(i);
    }

    return result;
}

// int BadCharacterPosition(const std::vector<int>& positions, int position) {
//     for (int i = std::ssize(positions) - 1 ; i >= 0; --i) {
//         if (positions[i] < position) {
//             return position - positions[i];
//         }
//     }
//     return position;
// }

std::vector<int> BoyerMoore(const std::string& text, const std::string& pattern) {
    auto lBigDash = LDashLinear(pattern);
    auto lSmallDash = LSmallDashLinear(pattern);
    auto rvalues = RightValues(pattern);

    std::vector<int> positions;

    int k = std::ssize(pattern) - 1;

    while (k < std::ssize(text)) {
        int i = std::ssize(pattern) - 1;
        int h = k;

        while (i >= 0 && pattern[i] == text[h]) {
            --i;
            --h;
        }

        auto oldK = k;

        if (i < 0) {
            // we have found a match!
            positions.push_back(h + 1);
            k += std::ssize(pattern) - lSmallDash[1];
        } else {
            int badCharacterRuleShift = rvalues[text[h]].Get(i);
            int goodSuffixShift = 1;
            {
                if (i == std::ssize(pattern) - 1) {
                    goodSuffixShift = 1;
                }
                else if (lBigDash[i + 1] > 0) {
                    goodSuffixShift = std::ssize(pattern) - lBigDash[i + 1] - 1;
                } else {
                    goodSuffixShift = std::ssize(pattern) - lSmallDash[i + 1];
                }
            }

            k += std::max(badCharacterRuleShift, goodSuffixShift);
        }
        assert(k > oldK);
    }

    return positions;
}

/////////////////////////////////////////////////////////////////////////

struct TTestPatternCase {
    std::string text;
    std::string pattern;
    std::vector<int> result;
};

bool CheckTest(const TTestPatternCase& t) {
    auto result = BoyerMoore(t.text, t.pattern);

    if (result != t.result) {
        std::cout << "Test case: text:" << t.text << " pattern:" << t.pattern << " failed!"
            << " expected: '" << Stringify(t.result)
            << "' got: '" << Stringify(result) << "'"
            << std::endl;
        return false;
    }
    return true;
}

int main()
{
    // CheckLSmalDash();

    // for (int i = 0; i < 100000; ++i) {
    //     auto testCase = TTestPatternCase {
    //         .text = GetRandomString(100000, 1),
    //         .pattern = GetRandomString(4, 1),
    //     };
    //     testCase.result = FindAllOccurrencesBruteForce(testCase.text, testCase.pattern);

    //     if (!CheckTest(testCase)) {
    //         return 1;
    //     }
    // }

    for (int i = 0; i < 100000; ++i) {
        auto testCase = TTestPatternCase {
            .text = GetRandomString(10000, 10),
            .pattern = GetRandomString(100, 10),
        };
        testCase.result = BoyerMoore(testCase.text, testCase.pattern);

        // if (!CheckTest(testCase)) {
        //     return 1;
        // }
    }

    // auto testCase = TTestPatternCase {
    //     .text = "dccbddccac",
    //     .pattern = "ddcc",
    // };
    // testCase.result = FindAllOccurrencesBruteForce(testCase.text, testCase.pattern);

    // if (!CheckTest(testCase)) {
    //     return 1;
    // }

    std::cout << "OK" << std::endl;
    return 0;
}