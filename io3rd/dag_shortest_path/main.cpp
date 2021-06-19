#include <iostream>
#include <vector>
#include <algorithm>

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


// ---------------- DFS and TopoSort
struct DFSContext {
    enum ECollor {
        White,
        Gray,
        Black
    };
    struct TVertextAttr {
        ECollor Seen = White;
    };
    DFSContext(const TGraph& g) {
        Attr.resize(g.Vertices.size());
    }
    std::vector<TVertextAttr> Attr;
};

using TVisitor = std::function<bool(TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp)>;
void DFSVisit(const TGraph& g, DFSContext& ctx, TGraph::TVertextId startingPoint, TVisitor visitor) {
    std::vector<TGraph::TVertextId> stack;
    stack.push_back(startingPoint);
    std::size_t ticker = 0;
    while (!stack.empty()) {
        auto current = stack.back();
        auto& attr = ctx.Attr[current];
        if (attr.Seen == DFSContext::Black) {
            stack.pop_back();
            // already visisted
            continue;
        }
        const auto& vertex = g.Vertices[current];
        if (!visitor(current, vertex.Value, attr.Seen == DFSContext::White, ticker++)) {
            // Interrapted by cliet code.
            break;
        }

        if (attr.Seen == DFSContext::Gray) {
            attr.Seen = DFSContext::Black;
            stack.pop_back();
            continue;
        }

        attr.Seen = DFSContext::Gray;

        for (auto v : vertex.Adj) {
            const auto& vattr = ctx.Attr[v.vertex];
            if (vattr.Seen == DFSContext::White) {
                stack.push_back(v.vertex);
            }
        }
    }
}

std::vector<TGraph::TVertextId> TopologicalSort(const TGraph& input) {
    std::vector<TGraph::TVertextId> finishingTimes;
    // Calculate finished times for original graph
    {
        DFSContext ctx(input);
        for (std::size_t i = 0; i < input.Vertices.size(); ++i) {
            if (ctx.Attr[i].Seen != DFSContext::White) {
                continue;
            }
            DFSVisit(input, ctx, i, [&finishingTimes](TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp) {
                if (!begin) {
                    finishingTimes.push_back(id);
                }
                return true;
            });
        }
    }
    std::reverse(finishingTimes.begin(), finishingTimes.end());
    return finishingTimes;
}
//--------

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

void dag_shortest_path(TGraph& graph, TGraph::TVertextId start, TAttrList& attr) {
    init_single_source(graph, start, attr);
    for (TGraph::TVertextId v : TopologicalSort(graph)) {
        for (const auto& adj : graph.Vertices[v].Adj) { 
            relax(attr, graph.Edges[adj.edge]);
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
    dag_shortest_path(graph, s, results);

    for (TGraph::TVertextId v = 0; v < results.size(); ++v) {
        std::cout << "verxtex: " << graph.Vertices[v].Value << " distance to s:" << results[v].distance << std::endl;
    }

    return 0;
}

