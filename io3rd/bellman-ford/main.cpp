#include <iostream>
#include <vector>
#include <algorithm>

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

using TGraph = TGraphGeneric<true>;

struct TSPAttr {
    int distance = 0;
    TGraph::TVertextId parent = 0; 
};

using TAttrList = std::vector<TSPAttr>;
constexpr int INF_WEIGHT = std::numeric_limits<int>::max() / 2;

void init_single_source(TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
    attr.resize(graph.Vertices.size());
    for (auto& a : attr) {
        a.parent = attr.size();
        a.distance = INF_WEIGHT;
    }
    attr[start].distance = 0;
}

void relax(TAttrList& attr, const TGraph::TEdge& edge) {
    auto& u1 = attr[edge.v1];
    auto& u2 = attr[edge.v2];
    if (u2.distance > u1.distance + edge.weight) {
        u2.distance = u1.distance + edge.weight;
        u2.parent = edge.v1;
    }
}

bool bellman_ford(TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
    init_single_source(graph, start, attr);
    for (int i = 0; i < graph.Vertices.size(); ++i) {
        for (auto& e : graph.Edges) {
            relax(attr, e);
        }
    }

    for (const auto& e : graph.Edges) {
        auto& u1 = attr[e.v1];
        auto& u2 = attr[e.v2];
        if (u2.distance > u1.distance + e.weight) {
            return false;
        }
    }
    return true;
}

int main() {
    TGraph graph;    
    auto s = graph.AddVertex("s");
    auto t = graph.AddVertex("t");
    auto x = graph.AddVertex("x");
    auto y = graph.AddVertex("y");
    auto z = graph.AddVertex("z");
    
    graph.AddEdge(s, t, 6);
    graph.AddEdge(s, y, 7);
    graph.AddEdge(t, x, 5);
    graph.AddEdge(t, z, -4);
    graph.AddEdge(t, y, 8);
    graph.AddEdge(x, t, -2);
    graph.AddEdge(y, x, -3);
    graph.AddEdge(y, z, 9);
    graph.AddEdge(z, x, 7);
    graph.AddEdge(z, s, 2);
    
    TAttrList results;
    bellman_ford(graph, s, results);

    for (TGraph::TVertextId v = 0; v < results.size(); ++v) {
        std::cout << "verxtex: " << graph.Vertices[v].Value << " distance to s:" << results[v].distance << std::endl;
    }

    return 0;
}