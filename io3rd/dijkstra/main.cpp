#include <iostream>
#include <vector>
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


using TGraph = TGraphGeneric<true>;
struct TSPAttr {
    int distance = 0;
    TGraph::TVertextId parent = 0;
    bool visited = false;
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

struct TVertexInfo {
    TGraph::TVertextId vertex = 0;
    int weight = 0;

    bool operator<(const TVertexInfo& r) const {
        return weight > r.weight;
    }
};

void dag_shortest_path(TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
    init_single_source(graph, start, attr);

    std::priority_queue<TVertexInfo> pq;
    pq.push(TVertexInfo{.vertex = start, .weight = 0});

    while(!pq.empty()) {
        auto v = pq.top().vertex;
        pq.pop();
        attr[v].visited = true;
        for (const auto& adj : graph.Vertices[v].Adj) { 
            relax(attr, graph.Edges[adj.edge]);
        }
        for (const auto& adj : graph.Vertices[v].Adj) {
            if (!attr[adj.vertex].visited) {
                pq.push(TVertexInfo{.vertex = adj.vertex, .weight = attr[adj.vertex].distance});
            }
        }
    }
}

int main() {
    TGraph graph;    
    auto s = graph.AddVertex("s");
    auto t = graph.AddVertex("t");
    auto x = graph.AddVertex("x");
    auto y = graph.AddVertex("y");
    auto z = graph.AddVertex("z");
    
    graph.AddEdge(s, t, 10);
    graph.AddEdge(s, y, 5);
    graph.AddEdge(t, x, 1);
    graph.AddEdge(t, y, 2);
    graph.AddEdge(x, z, 4);
    graph.AddEdge(y, x, 9);
    graph.AddEdge(y, z, 2);
    graph.AddEdge(y, t, 3);
    graph.AddEdge(z, x, 6);
    graph.AddEdge(z, s, 7);
    
    TAttrList results;
    dag_shortest_path(graph, s, results);

    for (TGraph::TVertextId v = 0; v < results.size(); ++v) {
        std::cout << "verxtex: " << graph.Vertices[v].Value << " distance from s:" << results[v].distance << std::endl;
    }

    return 0;
}

