#include <iostream>

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

constexpr int INF_WEIGHT = std::numeric_limits<int>::max() / 2;

template<bool Directed>
struct TGraphGeneric {

    // Ready to be a template parameter
    using TVertexValue = std::string;

    using TVertextId = std::size_t;

    using TAdjacencyMatrixRow = std::vector<int/*weight*/>;
    using TAdjacencyMatrix = std::vector<TAdjacencyMatrixRow>;

    struct TVertex {
        TVertexValue Value = 0;
    };

    using TVertices = std::vector<TVertex>;
    
    TVertices Vertices;
    TAdjacencyMatrix Adj;
    
    TVertextId AddVertex(const TVertexValue& v) {
        TVertextId id = Vertices.size();
        Vertices.emplace_back(TVertex{.Value = v});
        return id;
    }

    void AddEdge(TVertextId v1, TVertextId v2, int weight) {
        resize_adj();
        Adj[v1][v2] = weight;
        // Add back ref for undirected graph
        if constexpr (!Directed) {
            Adj[v2][v1] = weight;
        }
    }

    static void init_adj(TAdjacencyMatrix& adj, std::size_t n) {
        adj.resize(n);
        for (auto& row : adj) {
            row.resize(n, INF_WEIGHT);
        }
        for (std::size_t i = 0; i < n; ++i) {
            adj[i][i] = 0;
        }
    }


private:
    void resize_adj() {
        if (Adj.size() == Vertices.size()) {
            return;
        }
       init_adj(Adj, Vertices.size());
    }
};

using TGraph = TGraphGeneric<true>;

TGraph::TAdjacencyMatrix extend_shortest_path(const TGraph::TAdjacencyMatrix& curret_paths, const TGraph::TAdjacencyMatrix& adj) {
    const std::size_t vertexes_count = curret_paths.size();
    TGraph::TAdjacencyMatrix results;
    TGraph::init_adj(results, vertexes_count);
    for (std::size_t i = 0; i < vertexes_count; ++i) {
        for (std::size_t j = 0; j < vertexes_count; ++j) {
            for (std::size_t k = 0; k < vertexes_count; ++k) {
                results[i][j] = std::min(results[i][j], curret_paths[i][k] + adj[k][j]);
            }
        }
    }
    return results;
}

TGraph::TAdjacencyMatrix slow_all_pairs_shortest_path(const TGraph& graph) {
    auto paths = graph.Adj;
    for (int i = 1; i < graph.Vertices.size(); ++i) {
        paths = extend_shortest_path(paths, graph.Adj);
    }
    return paths;
}

TGraph::TAdjacencyMatrix faster_all_pairs_shortest_path(const TGraph& graph) {
    std::size_t n = graph.Vertices.size();
    std::size_t m = 1;
    auto paths = graph.Adj;
    while (m < n-1) {
        paths = extend_shortest_path(paths, paths);
        m = 2*m;
    }
    return paths;
}

TGraph::TAdjacencyMatrix floyd_warshall(const TGraph& graph) {
    const std::size_t vertexes_count = graph.Adj.size();
    TGraph::TAdjacencyMatrix current = graph.Adj;
    TGraph::TAdjacencyMatrix prev;
    TGraph::init_adj(prev, vertexes_count);
    for (std::size_t k = 0; k < vertexes_count; ++k) {
        prev.swap(current);
        for (std::size_t i = 0; i < vertexes_count; ++i) {
            for (std::size_t j = 0; j < vertexes_count; ++j) {
                current[i][j] = std::min(prev[i][j], prev[i][k] + prev[k][j]);
            }
        }

    }
    return prev;
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
    
    auto paths = floyd_warshall(graph);

    const auto& results = paths[s];
    for (TGraph::TVertextId v = 0; v < results.size(); ++v) {
        std::cout << "verxtex: " << graph.Vertices[v].Value << " distance to s:" << results[v] << std::endl;
    }

    return 0;
}
