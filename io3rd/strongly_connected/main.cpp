#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>


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
    };
    using TEdges = std::vector<TEdge>;
    
    TVertices Vertices;
    TEdges Edges;

    
    TVertextId AddVertex(const TVertexValue& v) {
        TVertextId id = Vertices.size();
        Vertices.emplace_back(TVertex{.Value = v});
        return id;
    }

    void AddEdge(TVertextId v1, TVertextId v2) {
        Edges.emplace_back(v1, v2);
        Vertices[v1].Adj.push_back(v2);
        // Add back ref for undirected graph
        if (!Directed) {
            Vertices[v2].Adj.push_back(v1);
        }
    }
};

using TGraph = TGraphGeneric<true>;
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
            const auto& vattr = ctx.Attr[v];
            if (vattr.Seen == DFSContext::White) {
                stack.push_back(v);
            }
        }
    }
}

TGraph Transpose(const TGraph& input) {
    TGraph result;
    result.Vertices.reserve(input.Vertices.size());
    result.Edges.reserve(input.Edges.size());
    for (const auto& v : input.Vertices) {
        result.AddVertex(v.Value);
    }
    for (const auto& e : input.Edges) {
        result.AddEdge(e.v2, e.v1);
    }
    return result;
} 

using TConnectedComponents = std::vector<std::vector<TGraph::TVertextId>>;

TConnectedComponents ConnectedComponents(const TGraph& input) {
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

    // Transpose Graph;
    TGraph transposed = Transpose(input);
    TConnectedComponents connectedComponents;
    DFSContext ctx(transposed);
    // Traverse transposed graph in reverse to finished times from step 1. 
    for (auto it = finishingTimes.rbegin(); it != finishingTimes.rend(); ++it) {
        if (ctx.Attr[*it].Seen != DFSContext::White) {
            continue;
        }
        auto& component = connectedComponents.emplace_back();
        DFSVisit(transposed, ctx, *it, [&component](TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp) {
            if (!begin) {
                component.push_back(id);
            }
            return true;
        });
    }
    return connectedComponents;
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


    graph.AddEdge(a, b);

    graph.AddEdge(b, e);
    graph.AddEdge(b, f);
    graph.AddEdge(b, c);

    graph.AddEdge(c, g);
    graph.AddEdge(c, d);

    graph.AddEdge(d, h);
    graph.AddEdge(d, c);

    graph.AddEdge(e, a);
    graph.AddEdge(e, f);

    graph.AddEdge(f, g);


    graph.AddEdge(g, f);
    graph.AddEdge(g, h);
    
    graph.AddEdge(h, h);
    
    TConnectedComponents components = ConnectedComponents(graph);

    for (const auto& comp : components) {
        for (auto v  : comp) {
            std::cout << graph.Vertices[v].Value << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}