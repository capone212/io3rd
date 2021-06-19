#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <queue>

template<bool Directed>
struct TGraphGeneric {

    // Ready to be a template parameter
    using TVertexValue = std::string;

    using TVertextId = std::size_t;
    using TEdgeId = std::size_t;

    struct TVertextPath {
        TVertextId vertex = 0;
        TEdgeId edge = 0;
    };
    using TAdjacencyList = std::vector<TVertextPath>;
    struct TVertex {
        TVertexValue Value = 0;
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
        auto edgeId = Edges.size();
        Edges.emplace_back(v1, v2, weight);
        Vertices[v1].Adj.push_back(TVertextPath{.vertex = v2, .edge=edgeId});
        // Add back ref for undirected graph
        if (!Directed) {
            Vertices[v2].Adj.push_back(TVertextPath{.vertex = v1, .edge=edgeId});
        }
    }

    void AddEdge(const TEdge& edge) {
        AddEdge(edge.v1, edge.v2, edge.weight);
    }
};

using TGraph = TGraphGeneric<false>;

TGraph prim(const TGraph& graph) {
    struct TAttr {
        bool visited = false;
    };

    std::vector<TAttr> attr;
    attr.resize(graph.Vertices.size());
    

    auto compare = [&graph](const TGraph::TVertextPath& l, const TGraph::TVertextPath& r) {
        return graph.Edges[l.edge].weight > graph.Edges[r.edge].weight;
    };
    std::priority_queue<TGraph::TVertextPath, std::vector<TGraph::TVertextPath>, decltype(compare)> pq(compare);
    auto add_vertex = [&](TGraph::TVertextId id) {
        attr[id].visited = true;
        for (auto& path : graph.Vertices[id].Adj) {
            if (!attr[path.vertex].visited) {
                pq.push(path);
            }
        }
    };
    // Prepare resulting graph (tree)
    TGraph result;
    result.Vertices.reserve(graph.Vertices.size());
    for (auto& v : graph.Vertices) {
        result.AddVertex(v.Value);
    }
    // init
    add_vertex(0);

    while (!pq.empty())
    {
        auto path = pq.top();
        pq.pop();
        if (attr[path.vertex].visited) {
            continue;
        }
        result.AddEdge(graph.Edges[path.edge]);
        add_vertex(path.vertex);
    }
    // std::priority
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
    
    auto minimum_spanning_tree = prim(graph);
    for (auto& e : minimum_spanning_tree.Edges) {
        std::cout << "Edge v1:" << e.v1 << " v2:" << e.v2 << " weight:" << e.weight << std::endl;
    }
    return 0;
}