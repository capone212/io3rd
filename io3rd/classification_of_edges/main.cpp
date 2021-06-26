#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <map>

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

using TGraph = TGraphGeneric<true>;

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



int main() {
   TGraph graph;    
    auto s = graph.AddVertex("s");
    auto t = graph.AddVertex("t");
    auto u = graph.AddVertex("u");
    auto v = graph.AddVertex("v");
    auto w = graph.AddVertex("w");
    auto x = graph.AddVertex("x");
    auto y = graph.AddVertex("y");
    auto z = graph.AddVertex("z");

    graph.AddEdge(s, w);
    graph.AddEdge(s, z);
    graph.AddEdge(z, w);
    graph.AddEdge(z, y);
    graph.AddEdge(w, x);
    graph.AddEdge(y, x);

    graph.AddEdge(x, z);
    graph.AddEdge(t, v);
    graph.AddEdge(t, u);

    graph.AddEdge(u, t);
    graph.AddEdge(u, v);

    graph.AddEdge(v, w);
    graph.AddEdge(v, s);
    
    
    TDFSContext ctx{graph};
    DFS(graph, ctx);

    for (std::size_t e = 0; e < graph.Edges.size(); ++e) {
        const auto& edge = graph.Edges[e];
        const auto& eattr = ctx.EdgeAttr[e];
        std::cout << "Edge: from " << graph.Vertices[edge.v1].Value << " to " << graph.Vertices[edge.v2].Value
            << " has type: " <<  to_string(eattr.edge_type)
            << std::endl;
    }
    return 0;
}

