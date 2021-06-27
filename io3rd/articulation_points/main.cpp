#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <map>
#include <assert.h>
#include <climits>
#include <set>
#include <algorithm>
#include <sstream>

template<bool Directed>
struct TGraphGeneric {

    // Ready to be a template parameter
    using TVertexValue = std::string;

    using TVertextId = std::size_t;
    using TEdgeId = std::size_t;

    constexpr static TVertextId NIL_VERTEX_ID = std::numeric_limits<TVertextId>::max();
    constexpr static TEdgeId NIL_EDGE_ID = std::numeric_limits<TEdgeId>::max();

    struct TVertextPath {
        TVertextId vertex = NIL_VERTEX_ID;
        TEdgeId edge = NIL_EDGE_ID;
    };
    using TAdjacencyList = std::vector<TVertextPath>;
    struct TVertex {
        TVertexValue Value = {};
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
        Vertices.push_back(TVertex{.Value = v});
        return id;
    }

    void AddEdge(TVertextId v1, TVertextId v2, int weight = 0) {
        auto edgeId = Edges.size();
        Edges.push_back({v1, v2, weight});
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
using TDirectedGraph = TGraphGeneric<true>;

const std::size_t INF_DISTANCE = std::numeric_limits<std::size_t>::max();

struct TLow {
    std::size_t back = INF_DISTANCE;
    std::size_t low = INF_DISTANCE;
    std::size_t discovery = INF_DISTANCE;
};

struct TDFSContext {
    enum struct ECollor {
        White,
        Gray,
        Black
    };
    
    struct TVertextAttr {
        ECollor Seen = ECollor::White;
        std::size_t discovery_ts = 0;
        std::size_t finish_ts = 0;
        TGraph::TVertextPath parent;

        bool IsLeath() const {
            assert(discovery_ts < finish_ts);
            return discovery_ts + 1 == finish_ts;
        }
    };

    enum struct EEdgeType {
        Unknown,
        Tree,
        Back,
        Forward,
        Cross
    };

    struct TEdgeAttr {
        EEdgeType edge_type = EEdgeType::Unknown;
    };

    TDFSContext(const TGraph& g) {
        Attr.resize(g.Vertices.size());
        EdgeAttr.resize(g.Edges.size());
    }
    std::vector<TVertextAttr> Attr;
    std::vector<TEdgeAttr> EdgeAttr;
    // Predecessor tree
    TDirectedGraph PredTree;
    std::vector<TLow> Lows;
};

using TVisitor = std::function<bool(TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp)>;

TDFSContext::EEdgeType get_edge_type(TGraph::TVertextId v1, TGraph::TVertextId v2, const TDFSContext& ctx) {
    using EEdgeType = TDFSContext::EEdgeType;
    switch (ctx.Attr[v2].Seen)
    {
    case TDFSContext::ECollor::White:
        return EEdgeType::Tree;
    case TDFSContext::ECollor::Gray:
        return EEdgeType::Back;
    case TDFSContext::ECollor::Black:
        return ctx.Attr[v1].discovery_ts > ctx.Attr[v2].discovery_ts ? EEdgeType::Cross : EEdgeType::Forward;  
    }
    throw std::runtime_error("unexpected edge type");
}

void DFSVisit(const TGraph& g, TDFSContext& ctx, TGraph::TVertextId startingPoint, TVisitor visitor) {
    std::vector<TGraph::TVertextId> stack;
    stack.push_back(startingPoint);
    std::size_t ticker = 0;
    while (!stack.empty()) {
        auto current = stack.back();
        auto& attr = ctx.Attr[current];
        if (attr.Seen == TDFSContext::ECollor::Black) {
            stack.pop_back();
            // already visisted
            continue;
        }
        const auto& vertex = g.Vertices[current];
        ticker++;
        if (!visitor(current, vertex.Value, attr.Seen == TDFSContext::ECollor::White, ticker)) {
            // Interrapted by cliet code.
            break;
        }

        if (attr.Seen == TDFSContext::ECollor::Gray) {
            attr.Seen = TDFSContext::ECollor::Black;
            attr.finish_ts = ticker;
            stack.pop_back();
            continue;
        }

        // Compute current edge type for white node
        if (attr.parent.edge != TGraph::NIL_EDGE_ID && ctx.EdgeAttr[attr.parent.edge].edge_type == TDFSContext::EEdgeType::Unknown) {
            ctx.EdgeAttr[attr.parent.edge].edge_type = get_edge_type(attr.parent.vertex, current, ctx);
        }
        attr.Seen = TDFSContext::ECollor::Gray;
        attr.discovery_ts = ticker;

        for (auto v : vertex.Adj) {
            auto& vattr = ctx.Attr[v.vertex];
            if (vattr.Seen == TDFSContext::ECollor::White) {
                // deffer computing edge state for scheduled vertex
                stack.push_back(v.vertex);
                vattr.parent = TGraph::TVertextPath{.vertex = current, .edge = v.edge};
            } else if (ctx.EdgeAttr[v.edge].edge_type == TDFSContext::EEdgeType::Unknown) {
                ctx.EdgeAttr[v.edge].edge_type = get_edge_type(current, v.vertex, ctx);
            }
        }
    }
}

std::vector<TLow> calc_all_lows(const TGraph& g, TDFSContext ctx);

void DFS(const TGraph& g, TDFSContext& ctx) {
    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        if (ctx.Attr[v].Seen != TDFSContext::ECollor::White) {
            continue;
        }
        DFSVisit(g, ctx, v, [](TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp){
            return true;
        });
    }
    // Color all remain edges
    for (TGraph::TEdgeId e = 0; e < g.Edges.size(); ++e) {
        const auto& edge = g.Edges[e];
        if (ctx.EdgeAttr[e].edge_type == TDFSContext::EEdgeType::Unknown) {
            ctx.EdgeAttr[e].edge_type = get_edge_type(edge.v1, edge.v2, ctx);
        }
    }
    // Create pred tree
    ctx.PredTree.Vertices.resize(g.Vertices.size());
    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        if (ctx.Attr[v].parent.vertex != TGraph::NIL_VERTEX_ID) {
            ctx.PredTree.AddEdge(ctx.Attr[v].parent.vertex, v);
        }
    }
    // Calc lows
    ctx.Lows = calc_all_lows(g, ctx);
}

std::string to_string(TDFSContext::EEdgeType e) {
    using EEdgeType = TDFSContext::EEdgeType;
    static const std::map<EEdgeType, std::string> mapping =  {
        { EEdgeType::Unknown, "Unknown"},
        { EEdgeType::Tree, "Tree"},
        { EEdgeType::Back, "Back"},
        { EEdgeType::Forward, "Forward"},
        { EEdgeType::Cross, "Cross"},
    };
    return mapping.at(e);
}

std::size_t calc_low(const TDirectedGraph& p_graph, TGraph::TVertextId v,  std::vector<TLow>& lows) {
    if (lows[v].low != INF_DISTANCE) {
        return lows[v].low;
    }
    auto low = std::min(lows[v].discovery, lows[v].back);
    for (auto child : p_graph.Vertices[v].Adj) {
        low = std::min(low, calc_low(p_graph, child.vertex, lows));
    }
    lows[v].low = low;
    return low;
}

std::vector<TLow> calc_all_lows(const TGraph& g, TDFSContext ctx) {
    // Calculate back leafs
    std::vector<TLow> lows;
    lows.resize(g.Vertices.size());
    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        lows[v].discovery = ctx.Attr[v].discovery_ts;
    }
    for (TGraph::TEdgeId e = 0; e < g.Edges.size(); ++e) {
        const auto& edge = g.Edges[e];
        const auto& attr = ctx.EdgeAttr[e];
        assert(attr.edge_type != TDFSContext::EEdgeType::Unknown); 
        if (attr.edge_type != TDFSContext::EEdgeType::Back) {
            continue;
        }
        auto l = std::min(ctx.Attr[edge.v1].discovery_ts, ctx.Attr[edge.v2].discovery_ts);
        lows[edge.v1].back = std::min(l, lows[edge.v1].back);
        lows[edge.v2].back = std::min(l, lows[edge.v2].back);
    }
    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        calc_low(ctx.PredTree, v, lows);
    }
    return lows;
}

using vertex_list_t = std::unordered_set<TGraph::TVertextId>;
vertex_list_t get_articulation_points(const TGraph& g, const TDFSContext& ctx) {
    vertex_list_t result;
    auto has_in_sync_child = [&](TGraph::TVertextId v) {
        auto parent_discovery = ctx.Lows[v].discovery; 
        for (auto child : ctx.PredTree.Vertices[v].Adj) {
            if (ctx.Lows[child.vertex].low >= parent_discovery) {
                return true;
            }
        }
        return false;
    };

    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        if (ctx.Attr[v].parent.vertex == TGraph::NIL_VERTEX_ID) {
            if (ctx.PredTree.Vertices[v].Adj.size() > 1) {
                result.insert(v);
            }
        } else if (has_in_sync_child(v)) {
            result.insert(v);
        }
    }
    return result;
}


std::vector<TGraph::TEdge> get_bridges(const TGraph& g, const TDFSContext& ctx) {
    std::vector<TGraph::TEdge> results;
    for (TGraph::TVertextId v = 0; v < g.Vertices.size(); ++v) {
        for (const auto& child : ctx.PredTree.Vertices[v].Adj) {
            if (ctx.Lows[child.vertex].low > ctx.Lows[v].discovery) {
                results.push_back(TGraph::TEdge{.v1 = v, .v2 = child.vertex});
            }
        }
    }
    return results;
}
using namespace std;

bool is_conncted(int n, const vector<vector<int>>& connections) {
    TGraph g;
    g.Vertices.resize(n);
    for (const auto& e : connections) {
        g.AddEdge(e.front(), e.back());
    }
    TDFSContext ctx{g};
    DFS(g, ctx);
    int parentless_count = 0;
    for (auto v : ctx.Attr) {
        if (v.parent.vertex == TGraph::NIL_VERTEX_ID) {
            ++parentless_count;
        }
    }
    return parentless_count == 1;
}

class Solution1 {
public:
    vector<vector<int>> criticalConnections(int n, const vector<vector<int>>& connections) {
        TGraph g;
        g.Vertices.resize(n);
        for (const auto& e : connections) {
            g.AddEdge(e.front(), e.back());
        }
        
        TDFSContext ctx{g};
        DFS(g, ctx);
        
        auto bridges = get_bridges(g, ctx);
        vector<vector<int>> results;
        for (auto e : bridges) {
            results.push_back({int(e.v1), int(e.v2)});
        }
        return results;
    }
};


class Solution2 {
public:
    vector<vector<int>>g;
    vector<vector<int>>res;
    vector<int>visited, dfn_ancestor, DFN, parent;
    int dfn = 0;
    void dfs(int v){
        visited[v] = true, DFN[v] = dfn++, dfn_ancestor[v] = INT_MAX;
        for(auto c : g[v]) {
            if(!visited[c]) {
                parent[c] = v;
                dfs(c);
                dfn_ancestor[v] = min(dfn_ancestor[v], dfn_ancestor[c]);
                if(dfn_ancestor[c] > DFN[v]) res.push_back({c, v});
            }
            else if (parent[v] != c)
                dfn_ancestor[v] = min(dfn_ancestor[v], DFN[c]);
        }
    }
    vector<vector<int>> criticalConnections(int n, const vector<vector<int>>& connect) {
        g.resize(n),visited.resize(n), dfn_ancestor.resize(n), parent.resize(n), DFN.resize(n);
        for(auto e : connect)
            g[e[0]].push_back(e[1]), g[e[1]].push_back(e[0]);
        dfs(0);
        return res;
    }
};


using connections_t = std::vector<std::vector<int>>;
std::pair<int, connections_t> generate_random(int max_count) { 
    int vertex_count = rand() % max_count + 3;
    int edge_count = rand() % (max_count * max_count) + vertex_count;
    std::set<std::pair<int, int>> edges;
    for (int i = 0; i < edge_count; ++i) {
        int v1 = rand() % vertex_count;
        int v2 = rand() % vertex_count;
        if (v1 !=  v2) {
            edges.insert({
                std::min(v1, v2),
                std::max(v1, v2)
            });
        }
    }
    connections_t connections;
    for (auto e : edges) {
        connections.push_back({e.first, e.second});
    }
    return {vertex_count, connections};
}

void canonical(connections_t& input) {
    for (auto& c : input) {
        std::sort(c.begin(), c.end());
    }
    std::sort(input.begin(), input.end(), [](const auto& l, const auto& r) {
        return std::tie(l[0], l[1]) < std::tie(r[0], r[1]);
    });
}

std::string print(const connections_t& input) {
    std::ostringstream s;
    for (const auto& c: input) {
        s << "{" << c.front() << "," << c.back() << "},";
    }
    return s.str();
} 


int main() {    

    for (int i = 0; i < 1000000; ++i) {
        const auto sample = generate_random(10);
        if (!is_conncted(sample.first, sample.second)) {
            continue;
        }
        auto r1 = Solution1().criticalConnections(sample.first, sample.second);
        auto r2 = Solution2().criticalConnections(sample.first, sample.second);
        canonical(r1);
        canonical(r2);
        if (r2.empty()) {
            continue;
        }
        if (!std::equal(r1.begin(), r1.end(), r2.begin(), r2.end())) {
            std::cout << "graph: n=" << sample.first << " edges:" << print(sample.second) << std::endl
                << "expected: " << print(r2) << std::endl
                << "got: " << print(r1) << std::endl;
            std::cout << "----------" << std::endl;
        }
    }
    return 0;
}

// int main() {

//     int n = 6;
//     std::vector<std::vector<int>> connections = {{0,2},{0,3},{1,3},{1,5},{2,3},{3,5},{4,5}};    

//     TGraph graph;    
//     for (int v = 0; v < n; ++v) {
//         graph.AddVertex(std::to_string(v));
//     }
//     for (const auto& e : connections) {
//         graph.AddEdge(e.front(), e.back());
//     }
    
//     TDFSContext ctx{graph};
//     DFS(graph, ctx);
//     std::vector<std::vector<int>> results;
//     auto bridges = get_bridges(graph, ctx);
//     for (auto e: bridges) {
//         std::cout << "Bridge from vertex: " << graph.Vertices[e.v1].Value << " to " <<  graph.Vertices[e.v2].Value << std::endl;
//     }   
//     std::cout << std::endl;
//     auto lows = calc_all_lows(graph, ctx);
//     for (int v = 0; v < n; ++v) {
//         std::cout << "Vertex v:" << v << " low:" << lows[v].low
//         << " discovery:" << lows[v].discovery 
//         << " finishing time:" << ctx.Attr[v].finish_ts
//         << " parent:" << ctx.Attr[v].parent.vertex 
//         << std::endl; 
//     }

//     auto articulations = get_articulation_points(graph, ctx);
//     for (auto v: articulations) {
//         std::cout << "Articulation vertex: " << graph.Vertices[v].Value << std::endl;
//     }

// //     auto bridges = get_bridges(graph, ctx);

//     return 0;
// }