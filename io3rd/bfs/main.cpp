#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
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

using TVisitor = std::function<bool(TGraph::TVertextId id, const TGraph::TVertexValue& value, int distance)>;
void BFS(const TGraph& g, TGraph::TVertextId startingPoint, TVisitor visitor) {
    struct TVertextRuntimeAttr {
        bool Seen = false;
        int Distance = 0;
    };
    std::deque<TGraph::TVertextId> queue;
    std::vector<TVertextRuntimeAttr> runtimeAttr(g.Vertices.size()); 
    queue.push_back(startingPoint);
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop_front();
        auto& attr = runtimeAttr[current];
        if (attr.Seen) {
            // already visisted
            continue;
        }
        attr.Seen = true;
        const auto& vertex = g.Vertices[current];
        if (!visitor(current, vertex.Value, attr.Distance)) {
            // Interrapted by cliet code.
            break;
        }

        for (auto v : vertex.Adj) {
            queue.push_back(v);
            runtimeAttr[v].Distance = attr.Distance + 1;
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
    
    
    BFS(g, r, [](TGraph::TVertextId id, const TGraph::TVertexValue& value, int distance) {
        std::cout << "Node id: " <<  id << " value: " << value << " distance:" << distance << std::endl;
        return true;
    });

    return 0;
}