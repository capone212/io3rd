#include <assert.h>
#include <cctype>
#include <vector>
#include <string>
#include <variant>
#include <iostream>

using namespace std;

struct TInteger {
    std::int64_t Value = 0;
};

struct TParOpen {
};

struct TParClosed {
};

struct TOpPlus {
};

struct TOpMinus {
};

struct TOpMultiply {
};
 
using TToken = std::variant<TInteger, TParOpen, TParClosed, TOpPlus, TOpMinus, TOpMultiply>;

struct TNode {
    TToken Token;
    TNode* parent = nullptr;
    TNode* left = nullptr;
    TNode* right = nullptr;
    
    template<typename TType>
    bool Is() const {
        return std::holds_alternative<TType>(Token);
    }

    void replace(TNode* child, TNode* new_child) {
        if (left == child) {
            left = new_child;
        } else if (right == child) {
            right = new_child;
        } else {
            assert(false);
        }
    }

    void add_left(TNode* node) {
        left = node;
        auto* old_parent = node ? node->parent : nullptr;
        node->parent = this;
        replace_parent(old_parent, node);
    }

    void add_right(TNode* node) {
        right = node;
        auto* old_parent = node ? node->parent : nullptr;
        node->parent = this;
        replace_parent(old_parent, node);
    }

    void replace_parent(TNode* old_parent, TNode* node) {
        if (!old_parent) {
            return;
        }
        old_parent->replace(node, this);
        assert(!parent);
        node->parent = old_parent;
    }
};


class Solution {
public:
    int calculate(string s) {  
        std::vector<TToken> tokenized = Tokenize(s);
        std::vector<TNode> treeHolder;
        treeHolder.reserve(2 * tokenized.size());
        TNode* current = nullptr;
        std::vector<TNode*> paranthesis;
        for (const auto& t : tokenized) {
            treeHolder.push_back(TNode{.Token = t});
            current = chain(current, &treeHolder.back());
        }
        current = get_root(current);
        print_tree(current);
        std::cout << std::endl;
        return compute(current);
    }

    TNode* get_root(TNode* node) {
        while (node && node->parent) {
            node = node->parent;
        }
        return node;
    }

    std::int64_t compute(TNode* current) {
        if (!current) {
            return 0;
        }
        if (current->Is<TOpPlus>()) {
            return compute_summ(current);
        }
        
        if (current->Is<TOpMinus>()) {
            return compute_difference(current);
        }

        if (current->Is<TInteger>()) {
            return std::get<TInteger>(current->Token).Value;
        }

        assert(false);
        return 0;
    }

    void print_tree(TNode* current) {
        if (!current) {
            return;
        }
        if (current->Is<TInteger>()) {
            std::cout << std::get<TInteger>(current->Token).Value;
            return;
        }

        if (current->Is<TOpPlus>()) {
            std::cout << "[";
            print_tree(current->left);
            std::cout << " + ";
            print_tree(current->right);
            return;
        }

        if (current->Is<TOpMinus>()) {
            std::cout << "[";
            print_tree(current->left);
            std::cout << " - ";
            print_tree(current->right);
            std::cout << "]";
            return;
        }
    }

    std::int64_t compute_summ(TNode* current) {
        std::int64_t summ = 0;
        if (current->left) {
            summ += compute(current->left);
        }

        if (current->right) {
            summ += compute(current->right);
        }
        return summ;
    }

    std::int64_t compute_difference(TNode* current) {
        std::int64_t result = 0;
        if (current->left) {
            result = compute(current->left);
        }

        if (current->right) {
            result -= compute(current->right);
        }
        return result;
    }
    
    
    TNode* chain(TNode* current, TNode* node) {
        if (current == nullptr) {
            return node;
        }
        
        // {
            
        // }
        
        if (bool isDigit = node->Is<TInteger>()) {
            if (current->left == nullptr) {
                current->add_left(node);
                return current;
            }

            if (current->right == nullptr) {
                current->add_right(node);
                return current;
            }
            assert(!isDigit);
            return nullptr;
        }

        if (node->Is<TOpPlus>() || node->Is<TOpMinus>()) {
            node->add_left(current);
            return node;
        }

        if (node->Is<TOpMultiply>()) {
            node->add_left(current);
            return node;
        }
        assert(false);
        return nullptr;
    }
    
    std::vector<TToken> Tokenize(const string& s) {
        std::vector<TToken> tokens;
        const auto* ptr = s.c_str();
        const auto* endStr = ptr + s.size();
        while (ptr != endStr) {
            if (std::isdigit(*ptr)) {
                char* pend = nullptr;
                auto value = std::strtol(ptr, &pend, 10);
                tokens.push_back(TInteger{value});
                ptr = pend;
                continue;
            }
            switch(*ptr) {
                case '(' :
                    tokens.push_back(TParOpen{});
                    break;
                case ')' :
                    tokens.push_back(TParClosed{});
                    break;
                case '+' :
                    tokens.push_back(TOpPlus{});
                    break;
                case '-' :
                    tokens.push_back(TOpMinus{});
                    break;
                case '*' :
                    tokens.push_back(TOpMultiply{});
                    break;
            }
            
            ++ptr;
        };
        
        return tokens;
    }
};


int main() {
    Solution solution;
    std::vector<std::pair<std::string, int>> test_cases = {
        // {"1+2", 3},
        // {"1+2+4", 7},
        // {"+2+1+2+4", 9},
        // {"1-2", -1},
        // {"2-1", 1},
        {"-2-1", -3},
        // {"2+1-10", -7},
    };
    for (const auto&[expr, expected] : test_cases) {
        const int result = solution.calculate(expr);
        if (result != expected) {
            std::cout << "expected:" << expected << " got:" << result << " test case:" << expr << std::endl;
            return 1;
        }
    }
    std::cout << "Ok" << std::endl;
    return 0;
}
