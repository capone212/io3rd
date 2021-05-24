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

    TNode* FindSet(TNode* node) {
        TNode* theParent = node;
        while (theParent->Parent != node) {
            theParent = theParent->Parent;
        }
        while (node != theParent) {
            auto* tmp = node;
            node = node->Parent;
            tmp->Parent = theParent;
        }
        return theParent;
    }

    void Union(TNode* x,  TNode* y) {
        Link(FindSet(x), FindSet(y));
    }

private:
    void Link(TNode* x,  TNode* y) {
        if (x->Rank > y->Rank) {
            y->Parent = x;
        } else {
            x->Parent = y;
        }
        if (x->Rank == y->Rank) {
            y->Rank += 1; 
        }
    }


private:
    TAccessor Accessor;
};

int main() {
    std::cout << "hello world!" << std::endl;
}
