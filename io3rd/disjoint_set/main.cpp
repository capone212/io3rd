#include <iostream>

template<typename T>
struct DSPrototype {
    T* Parent = nullptr;
    int Rank = 0;
};


template<typename TNode>
struct DisjointSet { 
    
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

int main() {
    // try to solve https://leetcode.com/problems/redundant-connection/submissions/ with DS
    std::cout << "hello world!" << std::endl;
}
