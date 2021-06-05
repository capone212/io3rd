#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>

template<typename T>
struct TDSPrototype {
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

    bool IsSameSet(TNode* node1, TNode* node2) {
        return FindSet(node1) == FindSet(node2); 
    }

    bool IsSameSet(TNode& node1, TNode& node2) {
        return IsSameSet(&node1, &node2);
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

template<typename TNode>
struct TDisjointSetBuff : public TDisjointSet<TNode> {
    using TParent = TDisjointSet<TNode>;
    
    explicit TDisjointSetBuff(std::size_t size) {
        Buffer.resize(size);
        for (auto& b : Buffer) {
            TParent::MakeSet(b);
        }
    }

    TNode& Get(std::size_t v) {
        return Buffer[v];
    }

    bool Union(std::size_t v1, std::size_t v2) {
        return TParent::Union(Get(v1), Get(v2));
    }

    bool IsSameSet(std::size_t v1, std::size_t v2) {
        return TParent::IsSameSet(Get(v1), Get(v2));
    }

    TNode* FindSet(std::size_t v) {
        return TParent::FindSet(&Get(v));
    }

private:
    std::vector<TNode> Buffer;
};

template<bool Directed>
struct TGraphGeneric {

    // Ready to be a template parameter
    using TVertexValue = std::string;

    using TVertextId = std::size_t;
    using TAdjacencyList = std::vector<TVertextId>;
    struct TVertex {
        TVertexValue Value;
        TAdjacencyList Adj;
    };

    using TVertices = std::vector<TVertex>;

    struct TEdge {
        TVertextId v1;
        TVertextId v2;
        int weight;
    };
    using TEdges = std::vector<TEdge>;
    
    TVertices Vertices;
    TEdges Edges;

    
    TVertextId AddVertex(const TVertexValue& v) {
        TVertextId id = Vertices.size();
        Vertices.emplace_back(TVertex{.Value = v});
        return id;
    }

    void AddEdge(TVertextId v1, TVertextId v2, int weight) {
        Edges.emplace_back(v1, v2, weight);
        Vertices[v1].Adj.push_back(v2);
        // Add back ref for undirected graph
        if (!Directed) {
            Vertices[v2].Adj.push_back(v1);
        }
    }

    void AddEdge(const TEdge& edge) {
        AddEdge(edge.v1, edge.v2, edge.weight);
    }
};

using TGraph = TGraphGeneric<false>;

TGraph kruskal(const TGraph& graph) {
    struct TAttr {
        TDSPrototype<TAttr> ds;
    };

    // Init disjoint set data structure
    TDisjointSetBuff<TAttr> disjointSet{ graph.Vertices.size() };

    // Prepare resulting graph (tree)
    TGraph result;
    result.Vertices.reserve(graph.Vertices.size());
    for(auto& v : graph.Vertices) {
        result.AddVertex(v.Value);
    }

    // Sort edges by weight asc
    auto edges = graph.Edges;
    std::sort(edges.begin(), edges.end(), [](const TGraph::TEdge& e1, const TGraph::TEdge& e2) {
        return e1.weight < e2.weight;
    });

    // body of the algo
    for (const auto& e : edges) {
        if (disjointSet.IsSameSet(e.v1, e.v2)) {
            continue;
        }
        disjointSet.Union(e.v1, e.v2);
        result.AddEdge(e);
    }

    return result;
}

int main() {
    TGraph graph;    
    auto a = graph.AddVertex("a");
    auto b = graph.AddVertex("b");
    auto c = graph.AddVertex("c");
    auto d = graph.AddVertex("d");
    auto e = graph.AddVertex("e");
    auto f = graph.AddVertex("f");
    auto g = graph.AddVertex("g");
    auto h = graph.AddVertex("h");
    auto i = graph.AddVertex("i");
    graph.AddEdge(a, b, 4);
    graph.AddEdge(b, c, 8);
    graph.AddEdge(c, d, 7);
    graph.AddEdge(d, e, 9);
    graph.AddEdge(e, f, 10);
    graph.AddEdge(g, f, 2);
    graph.AddEdge(h, g, 1);
    graph.AddEdge(a, h, 8);
    graph.AddEdge(h, i, 7);
    graph.AddEdge(i, c, 2);
    graph.AddEdge(c, f, 4);
    graph.AddEdge(i, g, 6);
    graph.AddEdge(d, f, 14);
    graph.AddEdge(b, h, 11);
    
    auto minimum_spanning_tree = kruskal(graph);
    for (auto& e : minimum_spanning_tree.Edges) {
        std::cout << "Edge v1:" << e.v1 << " v2:" << e.v2 << " weight:" << e.weight << std::endl;
    }
    return 0;
}