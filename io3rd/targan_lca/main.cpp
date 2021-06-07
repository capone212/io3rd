#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>

template<typename T>
struct DSPrototype {
    T* Parent = nullptr;
    int Rank = 0;
};


template<typename TNode>
struct TDisjointSet { 
    
    void MakeSet(TNode* node) {
        node->ds.Parent = node;
        node->ds.Rank = 0;
    }
    
    void MakeSet(TNode& node) {
        MakeSet(&node);
    }

    TNode* FindSet(TNode* node) {
        TNode* theParent = node;
        // Find representative of the set (father of fathers)
        while (theParent->ds.Parent != theParent) {
            theParent = theParent->ds.Parent;
        }
        // Path compression for nodes
        while (node != theParent) {
            auto* tmp = node;
            node = node->ds.Parent;
            tmp->ds.Parent = theParent;
        }
        return theParent;
    }

    bool Union(TNode* x,  TNode* y) {
        return Link(FindSet(x), FindSet(y));
    }
    
    bool Union(TNode& x,  TNode& y) {
        return Union(&x, &y);
    }

private:
    bool Link(TNode* x,  TNode* y) {
        if (x == y) {
            return false;
        } 
        if (x->ds.Rank > y->ds.Rank) {
            y->ds.Parent = x;
        } else {
            x->ds.Parent = y;
        }
        if (x->ds.Rank == y->ds.Rank) {
            y->ds.Rank += 1; 
        }
        return true;
    }
};

template<typename TKey, typename TNode>
struct TDisjointSetBuff : public TDisjointSet<TNode> {
    using TParent = TDisjointSet<TNode>;

    TNode& Get(TKey v) {
        return Buffer[v];
    }

    void MakeSet(TKey v) {
        TParent::MakeSet(Buffer[v]);
    }

    bool Union(TKey v1, TKey v2) {
        return TParent::Union(Get(v1), Get(v2));
    }

    bool IsSameSet(TKey v1, TKey v2) {
        return TParent::IsSameSet(Get(v1), Get(v2));
    }

    TNode* FindSet(TKey v) {
        return TParent::FindSet(&Get(v));
    }

private:
    std::unordered_map<TKey, TNode> Buffer;
};


struct TreeNode {
    int key = 0;
    TreeNode* parent = nullptr;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

using TTreeNodePtr = TreeNode*;

void insert(TTreeNodePtr& node, int key, TTreeNodePtr parent = nullptr) {
    if (node == nullptr) {
        node = new TreeNode{.key = key, .parent = parent};
        return;
    }
    if (key <= node->key) {
        return insert(node->left, key, node);
    } else {
        return insert(node->right, key, node);
    }
}

TTreeNodePtr search(TTreeNodePtr& node, int key) {
    if (node == nullptr || node->key == key) {
        return node;
    }
    if (key < node->key) {
        return search(node->left, key);
    } else {
        return search(node->right, key);
    }
}

void traverse(TTreeNodePtr node) {
    if (node == nullptr) {
        return;
    }
    traverse(node->left);
    std::cout << node->key << std::endl;
    traverse(node->right);
}

enum ENodeVisitStatus {
    ENV_UNVISITED,
    ENV_LEFT_VISITED,
    ENV_RIGHT_VISITED,
};

void traverse_non_recursive(TTreeNodePtr root, auto handleKey) {
    TTreeNodePtr current = root;
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
            handleKey(current);
        }

        // visit right
        if (state != ENV_RIGHT_VISITED && current->right) {
            current = current->right;
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

void destroy_tree(TTreeNodePtr root) {
    std::vector<TTreeNodePtr> nodes;
    traverse_non_recursive(root, [&nodes](TTreeNodePtr node){
        nodes.push_back(node);
    });
    for (TTreeNodePtr node : nodes) {
        delete node;
    }
}

struct TDSNode {
    DSPrototype<TDSNode> ds;
    TTreeNodePtr ancessor = nullptr;
};

struct LCAContext {
    using indext_t = std::pair<TTreeNodePtr, TTreeNodePtr>;
    using lca_map_t = std::map<indext_t, TTreeNodePtr>;
    using black_list_t = std::unordered_set<TTreeNodePtr>;

    lca_map_t results;
    black_list_t blacks;
    TDisjointSetBuff<TTreeNodePtr, TDSNode> disjoint_set;

    indext_t make_index(TTreeNodePtr l, TTreeNodePtr r) const {
        if (l < r) {
            std::swap(l,r);
        }
        return std::make_pair(l, r);
    }

    void AddResults(TTreeNodePtr l, TTreeNodePtr r, TTreeNodePtr lca) {
        results[make_index(l, r)] = lca;
    }
    
    TTreeNodePtr GetLCA(TTreeNodePtr l, TTreeNodePtr r) const {
        auto index = make_index(l, r);
        return results.at(index);
    }
};

void LCA(TTreeNodePtr root, LCAContext& ctx) {
    ctx.disjoint_set.MakeSet(root);
    ctx.disjoint_set.FindSet(root)->ancessor = root;
    for (auto child : {root->left, root->right}) {
        if (child == nullptr) {
            continue;
        }
        LCA(child, ctx);
        ctx.disjoint_set.Union(root, child);
        ctx.disjoint_set.FindSet(root)->ancessor = root;
    }
    ctx.blacks.insert(root);
    for (auto v : ctx.blacks) {
        ctx.AddResults(root, v, ctx.disjoint_set.FindSet(v)->ancessor);
    }
}

int main() {
    const std::vector<int> values = {4, 5, 1, 2, 5, 3, 10};
    TTreeNodePtr root = nullptr;
    for (int v : values) {
        insert(root, v);
    }

    LCAContext ctx;
    LCA(root, ctx);

    for (const auto& [index, lca] : ctx.results) {
        std::cout << "Node: " << index.first->key << ", " << index.second->key << " LCA:" << lca->key << std::endl; 
    }

    destroy_tree(root);
    return 0;
}