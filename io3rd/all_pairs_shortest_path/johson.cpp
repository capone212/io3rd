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

void init_single_source(const TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
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

bool bellman_ford(const TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
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

void dijkstra(const TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
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

using TAdjacencyMatrixRow = std::vector<int/*weight*/>;
using TAdjacencyMatrix = std::vector<TAdjacencyMatrixRow>;

static void init_adj(TAdjacencyMatrix& adj, std::size_t n) {
    adj.resize(n);
    for (auto& row : adj) {
        row.resize(n, INF_WEIGHT);
    }
    for (std::size_t i = 0; i < n; ++i) {
        adj[i][i] = 0;
    }
}

TAdjacencyMatrix johnson(const TGraph& input) {
    TGraph graph = input;
    // Add transitive start vertex
    TGraph::TVertextId s0 = graph.AddVertex("s0");
    for (TGraph::TVertextId v = 0; v < s0; ++v) {
        // Add zero weighted edge from new vertext to every other
        graph.AddEdge(s0, v, 0);
    }

    TAttrList heights;
    if (!bellman_ford(graph, s0, heights)) {
        // Graph contains negative weight cycles
        return {};
    }
    // re-weight graph with height functions: w'(u,v) = w(u,v) + h(u) - h(v).
    // By triangle enequality h(v) <= h(u) + w(u,v), so w'(u,v) defined above is >=0.
    // Remember that h(u) and h(v) is shortest paths from s0.
    for (auto& e : graph.Edges) {
        e.weight = e.weight + heights[e.v1].distance - heights[e.v2].distance;
    }

    TAdjacencyMatrix result;
    init_adj(result, input.Vertices.size());
    for (TGraph::TVertextId v1 = 0; v1 < input.Vertices.size(); ++v1) {
        // Compute shortest paths from this vertext (v1) to all others
        // for re-weighted graph
        TAttrList weights;
        dijkstra(graph, v1, weights);
        // Re-calculate correct values for input graph
        for (TGraph::TVertextId v2 = 0; v2 < input.Vertices.size(); ++v2) {
            result[v1][v2] = weights[v2].distance + heights[v2].distance - heights[v1].distance;
        }
    }
    return result;
}


int main() {
    TGraph graph;    
    auto v1 = graph.AddVertex("1");
    auto v2 = graph.AddVertex("2");
    auto v3 = graph.AddVertex("3");
    auto v4 = graph.AddVertex("4");
    auto v5 = graph.AddVertex("5");
    
    graph.AddEdge(v1, v2, 3);
    graph.AddEdge(v1, v5, -4);
    graph.AddEdge(v1, v3, 8);
    graph.AddEdge(v2, v5, 7);
    graph.AddEdge(v2, v4, 1);
    graph.AddEdge(v3, v2, 4);
    graph.AddEdge(v4, v3, -5);
    graph.AddEdge(v4, v1, 2);
    graph.AddEdge(v5, v4, 6);
    
    auto paths = johnson(graph);

    const auto& results = paths[v1];
    for (TGraph::TVertextId v = 0; v < results.size(); ++v) {
        std::cout << "verxtex: " << graph.Vertices[v].Value << " distance to s:" << results[v] << std::endl;
    }

    return 0;
}

