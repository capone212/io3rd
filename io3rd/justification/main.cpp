#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    using line_t = vector<string>;
    using lines_list_t = std::vector<line_t>;
    
    vector<string> fullJustify(const vector<string>& words, int maxWidth) {
        auto lines = split_lines(words, maxWidth);
        int count = lines.size();
        vector<string> results;
        for (const auto& l : lines) {
            if (l.empty()) {
                continue;
            }
            std::string s;
            if (--count != 0 && l.size() != 1) {
                s = format_center(l, maxWidth);
            } else {
                s = format_left(l, maxWidth);
            }
            results.push_back(s);
        }
        return results;
    }
    
    lines_list_t split_lines(const vector<string>& words, int maxWidth) {
        lines_list_t list;
        list.emplace_back();
        int current_line = 0;
        for (const auto& w : words) {
            current_line = current_line + w.length() + (current_line ?  1 : 0);
            if (current_line > maxWidth) {
                list.emplace_back();
                current_line = w.length();
            }
            list.back().push_back(w);
        }
        return list;
    }
    
    std::string format_center(const line_t& line, int maxWidth) {
        int total_spaces = maxWidth - count(line);
        int at_least = total_spaces / (line.size() - 1);
        int remainder = total_spaces % (line.size() - 1);
        std::string result;
        for (int i = 0; i < line.size(); ++i) {
            result += line[i];
            if (i != line.size() - 1) {
                if (auto length = at_least + (remainder > 0 ? 1 : 0); length > 0) {
                    result.append(length, ' ');
                    --remainder;
                }   
            }
        }
        return result;
    }
    
    std::string format_left(const line_t& line, int maxWidth) {
        int total_spaces = maxWidth - count(line);
        std::string result;
        for (int i = 0; i < line.size(); ++i) {
            result += line[i];
            if (i != line.size() - 1) {
                result.append(1, ' ');
                --total_spaces;
            }
        }
        if (total_spaces > 0) {
            result.append(total_spaces, ' ');
        }
        return result;
    }
    
    int count(const line_t& line) {
        int summ = 0;
        for (const auto& w : line) {
            summ += w.length();
        }
        return summ;
    }
    
    
};

int main() {
    Solution s;
    // ["What","must","be","acknowledgment","shall","be"]
    auto result = s.fullJustify({"What","must","be","acknowledgment","shall","be"}, 16);
    for (const auto& s : result) {
        std::cout << s << std::endl;
    }
    return 0;
}