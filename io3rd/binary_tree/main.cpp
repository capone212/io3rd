#include <iostream>
#include <vector> 

struct TNode {
    int key = 0;
    TNode* parent = nullptr;
    TNode* left = nullptr;
    TNode* rigth = nullptr;
};

using TNodePtr = TNode*;

void insert(TNodePtr& node, int key, TNodePtr parent = nullptr) {
    if (node == nullptr) {
        node = new TNode{.key = key, .parent = parent};
        return;
    }
    if (key <= node->key) {
        return insert(node->left, key, node);
    } else {
        return insert(node->rigth, key, node);
    }
}

void traverse(TNodePtr node) {
    if (node == nullptr) {
        return;
    }
    traverse(node->left);
    std::cout << node->key << std::endl;
    traverse(node->rigth);
}

enum ENodeVisitStatus {
    ENV_UNVISITED,
    ENV_LEFT_VISITED,
    ENV_RIGHT_VISITED,
};

void traverse_non_recursive(TNodePtr root, auto handleKey) {
    TNodePtr current = root;
    ENodeVisitStatus state = ENV_UNVISITED;
    while (true) {
        if (current == nullptr) {
            break;
        }

        // visit left child
        if (state == ENV_UNVISITED && current->left) {
            current = current->left;
            continue;
        }

        // visit the node
        if (state != ENV_RIGHT_VISITED) {
            handleKey(current->key);
        }

        // visit right
        if (state != ENV_RIGHT_VISITED && current->rigth) {
            current = current->rigth;
            state = ENV_UNVISITED;
            continue;
        }

        // go up to parent
        auto* parent = current->parent;
        if (parent) {
            state = (parent->left == current) ? ENV_LEFT_VISITED : ENV_RIGHT_VISITED;
        }
        current = parent;
    }
}


int main() {
    const std::vector<int> values = {4, 5, 1, 2, 5, 3, 10};
    TNodePtr root = nullptr;
    for (int v : values) {
        insert(root, v);
    }
    traverse(root);
    std::cout << "Traverse non recursive" << std::endl; 
    traverse_non_recursive(root, [](int key) {
        std::cout << key << std::endl;
    });
    return 0;
}
