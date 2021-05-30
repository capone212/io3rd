#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <deque>


struct TGraph {

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
        Vertices[v2].Adj.push_back(v1);
    }
};

using TVisitor = std::function<bool(TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp)>;
void DFS(const TGraph& g, TGraph::TVertextId startingPoint, TVisitor visitor) {
    struct TVertextAttr {
        enum ECollor {
            White,
            Gray,
            Black
        };
        ECollor Seen = White;
    };
    std::vector<TGraph::TVertextId> stack;
    std::vector<TVertextAttr> runtimeAttr(g.Vertices.size());
    stack.push_back(startingPoint);
    std::size_t ticker = 0;
    while (!stack.empty()) {
        auto current = stack.back();
        auto& attr = runtimeAttr[current];
        if (attr.Seen == TVertextAttr::Black) {
            stack.pop_back();
            // already visisted
            continue;
        }
        const auto& vertex = g.Vertices[current];
        if (!visitor(current, vertex.Value, attr.Seen == TVertextAttr::White, ticker++)) {
            // Interrapted by cliet code.
            break;
        }

        if (attr.Seen == TVertextAttr::Gray) {
            attr.Seen = TVertextAttr::Black;
            stack.pop_back();
            continue;
        }

        attr.Seen = TVertextAttr::Gray;

        for (auto v : vertex.Adj) {
            const auto& vattr = runtimeAttr[v];
            if (vattr.Seen == TVertextAttr::White) {
                stack.push_back(v);
            }
        }
    }
}


int main() {
    TGraph g;    
    auto r = g.AddVertex("r");
    auto v = g.AddVertex("v");
    auto s = g.AddVertex("s");
    auto w = g.AddVertex("w");
    auto t = g.AddVertex("t");
    auto x = g.AddVertex("x");
    auto u = g.AddVertex("u");
    auto y = g.AddVertex("y");

    g.AddEdge(r, v);
    g.AddEdge(r, s);
    g.AddEdge(s, w);
    g.AddEdge(w, t);
    g.AddEdge(w, x);
    g.AddEdge(t, u);
    g.AddEdge(t, x);
    g.AddEdge(x, u);
    g.AddEdge(x, y);
    g.AddEdge(u, y);
    
    DFS(g, r, [](TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp) {
        std::cout << "Node id:" <<  id << " value:" << value << " begin:" << begin << " timestamp:" << timestamp <<  std::endl;
        return true;
    });

    // auto startpoint = r;
    // auto destination = w;
    // std::unordered_map<TGraph::TVertextId, TGraph::TVertextId> relations;
    // DFS(g, r, [destination, &relations](TGraph::TVertextId id, const TGraph::TVertexValue& value, bool begin, std::size_t timestamp) {
    //     relations[id] = parent;
    //     return id != destination;
    // });
    // auto current = destination;
    // while (current != startpoint) {
    //     std::cout << current << " ";
    //     current = relations.at(current); 
    // }
    // std::cout << startpoint << std::endl;

    return 0;
}