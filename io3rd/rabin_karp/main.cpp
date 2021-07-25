#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <iterator>

const std::int32_t QPRIME = 2'750'159;

std::int32_t naive_power_mode_q(std::int32_t value, std::int32_t power, std::int32_t q) {
    std::size_t result = 1;
    for (int i = 0; i < power; ++i) {
        result = (result * value) % q;
    }
    return result;
}

std::int32_t minus_modulo_q(std::int32_t l, std::int32_t r, std::int32_t q) {
    auto res = (l - r) % q;
    if (res < 0) {
        return q + res;
    }
    return res;
}
std::vector<std::int32_t> rabin_karp_matcher(const std::string_view& text, const std::string_view& pattern) {
    if (text.size() < pattern.size()) {
        return {};
    }
    const std::int32_t CHAR_DIM = 256;
    const std::int32_t m = pattern.size(); 
    const std::int32_t h = naive_power_mode_q(CHAR_DIM, int(pattern.size()) - 1, QPRIME);
    std::int32_t p = 0;
    std::int32_t ts = 0;
    for (int i = 0; i < m; ++i) {
        p = (CHAR_DIM * p + pattern[i]) % QPRIME;
        ts = (CHAR_DIM * ts + text[i]) % QPRIME;
    }
    std::vector<std::int32_t> indexes;
    for (int s = 0; s <= text.size() - m; ++s) {
        std::cout << "p:" << p << " ts:" << ts << " h:" << h << std::endl;
        if (p==ts && pattern == text.substr(s, m)) {
            indexes.push_back(s);
        }
        if (s < text.size() - m) {
            ts = (CHAR_DIM * minus_modulo_q(ts, text[s] * h, QPRIME) + text[s + m]) % QPRIME;
        }
    }
    return indexes;
}

std::string to_string(const std::vector<int>& nums) {
    std::ostringstream oss;
    if (!nums.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(nums.begin(), nums.end()-1,
            std::ostream_iterator<int>(oss, ","));

        // Now add the last element with no delimiter
        oss << nums.back();
    }
    return oss.str();
}

int main() {
    struct TTestCase {
        std::string p;
        std::string text;
        std::vector<int> res;
    };
    std::vector<TTestCase> cases = {
        {.p="aa", .text="aaaa", .res={0,1,2}},
        {.p="ab", .text="aaab", .res={2}},
        {.p="ab", .text="ab", .res={0}},
        {.p="ac", .text="ab", .res={}},
        {.p="issi", .text="mississippi", .res={1, 4}}
    };

    for (const auto& t : cases) {
        auto result = rabin_karp_matcher(t.text, t.p);
        if (result.size() != t.res.size() || !std::equal(result.begin(), result.end(), t.res.begin())) {
            std::cout << "Failed test:" << t.text 
            << " expected:" << to_string(t.res)
            << " got:" << to_string(result) << std::endl;
            return 1;
        }
    }
    std::cout << "OK" << std::endl;
    return 0;
}
