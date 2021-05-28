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

using TVisitor = std::function<bool(TGraph::TVertextId id, const TGraph::TVertexValue& value, TGraph::TVertextId parent, int distance)>;
void BFS(const TGraph& g, TGraph::TVertextId startingPoint, TVisitor visitor) {
    struct TVertextRuntimeAttr {
        bool Seen = false;
        int Distance = 0;
        TGraph::TVertextId Parent = 0;
    };
    std::deque<TGraph::TVertextId> queue;
    std::vector<TVertextRuntimeAttr> runtimeAttr(g.Vertices.size());
    runtimeAttr[startingPoint].Parent = startingPoint;
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
        if (!visitor(current, vertex.Value, attr.Parent, attr.Distance)) {
            // Interrapted by cliet code.
            break;
        }

        for (auto v : vertex.Adj) {
            queue.push_back(v);
            auto& vattr = runtimeAttr[v];
            vattr.Distance = attr.Distance + 1;
            vattr.Parent = current;
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
    
    BFS(g, r, [](TGraph::TVertextId id, const TGraph::TVertexValue& value, TGraph::TVertextId parent, int distance) {
        std::cout << "Node id:" <<  id << " Parent:" << parent << " value:" << value << " distance:" << distance << std::endl;
        return true;
    });

    auto startpoint = r;
    auto destination = w;
    std::unordered_map<TGraph::TVertextId, TGraph::TVertextId> relations;
    BFS(g, r, [destination, &relations](TGraph::TVertextId id, const TGraph::TVertexValue& value, TGraph::TVertextId parent, int distance) {
        relations[id] = parent;
        return id != destination;
    });
    auto current = destination;
    while (current != startpoint) {
        std::cout << current << " ";
        current = relations.at(current); 
    }
    std::cout << startpoint << std::endl;

    return 0;
}