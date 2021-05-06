#include <string_view>
#include <unordered_map>

std::string_view move_front(std::string_view v) {
     v.remove_prefix(1);
     return v;
}

class Solution {
public:
    bool isMatch(string s, string p) {
        return bottom_up(s, p);
    }

    bool brute_force(std::string_view s, string_view p) {
        if (s.empty()) {
            return match_empty(p);
        }
        if (p.empty()) {
            return false;
        }
        if (p.front() == '*') {
            return brute_force(move_front(s), p) || brute_force(s, move_front(p));
        } else if (match(s.front(), p.front())) {
            return brute_force(move_front(s), move_front(p));
        }
        return false;
    }

    bool top_down(std::string_view s, string_view p) {
        if (s.empty()) {
            return match_empty(p);
        }
        if (p.empty()) {
            return false;
        }
        p = skip_wilds(p);
        const std::string index = std::string(s) + std::string("#") + std::string(p);
        if (auto it = m_td_memo.find(index); it != m_td_memo.end()) {
            return it->second;
        }
        bool result = false;
        if (p.front() == '*') {
            result = top_down(move_front(s), p) || top_down(s, move_front(p));
        } else if (match(s.front(), p.front())) {
            result = top_down(move_front(s), move_front(p));
        }
        m_td_memo[index] = result;
        return result;
    }

    bool bottom_up(std::string_view s, string_view p) {
        if (s.empty()) {
            return match_empty(p);
        }
        if (p.empty()) {
            return false;
        }
        m_bu_memo.resize(p.size() + 1);
        for (int i = 0; i < p.size() + 1; ++i) {
            m_bu_memo[i].resize(s.size()+1, false);
        }

        for (int i = 0; i < p.size() + 1; ++i) {
            m_bu_memo[i][s.size()] = match_empty(p.substr(i));
        }

        for (int i = 0; i < s.size() + 1; ++i) {
            m_bu_memo[p.size()][i] = (i = s.size());
        }

        for (int i = p.size() - 1; i >= 0; --i) {
            for (int j = s.size() - 1; j >= 0; --j) {
                bool result = false;
                if (p[i] == '*') {
                    result = m_bu_memo[i][j+1] || m_bu_memo[i+1][j];
                } else if (match(s[j], p[i])) {
                    result = m_bu_memo[i+1][j+1];
                }
                m_bu_memo[i][j] = result;
            }
        }
        return m_bu_memo[0][0];
    }

    inline bool match(char left, char right) {
        return left == right || right == '?';
    }
    std::string_view skip_wilds(std::string_view p) {
        while (p.size() > 1 && p[0] == '*' && p[0] == p[1]) {
            p.remove_prefix(1);
        }
        return p;
    }

    bool match_empty(std::string_view p) {
        while (!p.empty()) {
            if (p.front() != '*') {
                return false;
            }
            p.remove_prefix(1);
        }
        return true;
    }

private:
    std::unordered_map<std::string, bool> m_td_memo;
    std::vector<std::vector<bool>> m_bu_memo;
};

