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
    bool Closed = false;
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
        assert(node != this);
        left = node;
        auto* old_parent = node ? node->parent : nullptr;
        node->parent = this;
        replace_parent(old_parent, node);
    }

    void add_right(TNode* node) {
        assert(node != this);
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
        parent = old_parent;
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
            // print_from_root(current);
        }
        current = get_root(current);
        // print_from_root(current);
        return compute(current);
    }

    void print_from_root(TNode* current) {
        current = get_root(current);
        print_tree(current);
        std::cout << std::endl;
    }

    TNode* get_root(TNode* node) {
        while (node && node->parent) {
            node = node->parent;
        }
        return node;
    }

    TNode* get_right_most_op(TNode* node) {
        while (node && node->right) {
            if (node->right->Is<TInteger>()) {
                break;
            }
            node = node->right;
        }
        return node;
    }

    TNode* get_right_most_digit(TNode* node) {
        while (node && node->right) {
            node = node->right;
        }
        if (node->Is<TInteger>()) {
            return node;
        }
        return nullptr;
    }

    TNode* get_first_par_open(TNode* node) {
        while (node && node->parent) {
            if (is_open_parenthesis(node)) {
                return node;
            }
            node = node->parent;
        }
        assert(node);
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
        if (current->Is<TOpMultiply>()) {
            return compute_product(current);
        }

        if (current->Is<TInteger>()) {
            return std::get<TInteger>(current->Token).Value;
        }

        if (current->Is<TParOpen>()) {
            assert(!current->left);
            return compute(current->right);
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
            std::cout << "]";
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

        if (current->Is<TOpMultiply>()) {
            std::cout << "[";
            print_tree(current->left);
            std::cout << " * ";
            print_tree(current->right);
            std::cout << "]";
            return;
        }

        if (current->Is<TParOpen>()) {
            std::cout << " ( ";
            print_tree(current->right);
            assert(current->left == nullptr);
            std::cout << " ) ";
            return;
        }
        assert(false);
    }

    bool is_open_parenthesis(TNode* current) {
        return current->Is<TParOpen>() && !std::get<TParOpen>(current->Token).Closed;
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

    std::int64_t compute_product(TNode* current) {
        std::int64_t product = 1;
        if (current->left) {
            product *= compute(current->left);
        }

        if (current->right) {
            product *= compute(current->right);
        }
        return product;
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
        
        if (node->Is<TInteger>()) {
            if (auto* rightmost = get_right_most_op(current); rightmost && rightmost->right == nullptr) {
                rightmost->add_right(node);
                if (rightmost->Is<TParOpen>()) {
                    return node;
                }
                return current;
            }
            assert(false);
            return nullptr;
        }

        if (node->Is<TOpPlus>() || node->Is<TOpMinus>()) {
            if (auto* rightmost = get_right_most_op(current); !current->Is<TInteger>() && !rightmost->right) {
                rightmost->add_right(node);
                return current;
            }
            if (is_open_parenthesis(current)) {
                node->add_left(current->right);
            } else {
                node->add_left(current);
            }
            return node;
        }

        if (node->Is<TOpMultiply>()) {
            if (auto* righmost = get_right_most_digit(current);  !current->Is<TInteger>() && righmost) {
                node->add_left(righmost);
                return current;
            }
            node->add_left(current);
            return node;
        }
        if (node->Is<TParOpen>()) {
            if (auto* rightmost = get_right_most_op(current); rightmost && rightmost->right == nullptr) {
                rightmost->add_right(node);
                return node;
            }
            assert(false);
            return nullptr;
        }
        if (node->Is<TParClosed>()) {
           auto* result = get_first_par_open(current);
           auto& isClosed = std::get<TParOpen>(result->Token).Closed;
           assert(!isClosed);
           isClosed = true;
           return get_first_par_open(result);
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
        // {"-2-1", -3},
        // {"2+1-10", -7},
        // {"2*1", 2},
        // {"-2*1", -2},
        // {"-2*-1", 2},
        // {"-2*--1", -2},
        // {"--2*--1", 2},
        // {"2*3+4", 10},
        // {"2+3*4", 14},
        // {"2*2+3*4", 16},
        // {"-2*-2*-3*4", -48},
        // {"-2*-2*-3*-4", 48},
        // {"---2*--2*---3", 12},
        // {"(1+2) + 3", 6},
        // {"(-1+2) + 3", 4},
        // {"(--1+2) + 3", 6},
        // {"-(-1+2) + 3", 2},
        // {"- (3 + (4 + 5))", -12},
        // {"-(1) + 3", 2},
        // {"--(-1+2) + 3", 4},
        // {"--(-(1+2))- (1)", -4},
    };

    // TODO: add paranthesis
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
