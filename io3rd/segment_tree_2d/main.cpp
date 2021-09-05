template <typename TValue, typename TTraits>
struct TSegmentTree {
public:
    
    TSegmentTree() = default;
    
    // WARN: values size should be power of two!
    explicit TSegmentTree(std::vector<TValue>&& values)
        : ElementsCount(values.size())
    {
        // TODO: ElementsCount is power of two > 0
        Tree.resize(ElementsCount * 2 - 1);
        std::copy(
            std::make_move_iterator(values.begin()),
            std::make_move_iterator(values.end()),
            Tree.begin() + ElementsCount - 1
        );
        for (int i = ElementsCount - 2; i >= 0; --i) {
            Tree[i] = Traits.CombineValues(Tree[2*i+1], Tree[2*i+2]);
        }
    }
    using TTraitsType = TTraits;
    using TQueryResultType = typename TTraits::TQueryResultType;
    
    template<typename ...TIndecies>
    TQueryResultType QueryRange(int lower, int upper, TIndecies... aditionalIndecies) const {
        return QueryRangeImpl(0, 0, ElementsCount-1, lower, upper, aditionalIndecies...);
    }
    
    template<typename TFinalValue, typename ...TIndecies>
    void Update(const TFinalValue& val, int index,  TIndecies... aditionalIndecies) {
        index = ElementsCount + index - 1;
        Traits.ContinueUpdate(Tree[index], val, aditionalIndecies...);
        while (index > 0) {
            index = (index - 1) / 2;
            Tree[index] = Traits.CombineValues(Tree[2*index+1], Tree[2*index+2]);
        }
    }
    
    template<typename ...TIndecies>
    TQueryResultType QueryRangeImpl(int treeIndex, int segLower, int segUpper, int lower, int upper, TIndecies... aditionalIndecies) const {
        TQueryResultType result = Traits.Neutral();
        if (segLower > upper || segUpper < lower) {
            // No intersection
            return result;
        }
        if (lower <= segLower && upper >= segUpper) {
            // Complete overlap
            return Traits.ContinueQuery(Tree[treeIndex], aditionalIndecies...);
        }
        
        // Here we have partial overlap. Possible cases
        // [seg_lower {lower  seg_upper] upper}
        // {lower [seg_lower, upper} seg_upper]
        int segMiddle = (segUpper + segLower) / 2;
        if (segMiddle >= lower) {
            TQueryResultType ltmp = QueryRangeImpl(2 * treeIndex + 1, segLower, segMiddle, lower, upper, aditionalIndecies...);
            result = Traits.CombineResults(result, ltmp);
        }
        
        if (segMiddle < upper) {
            TQueryResultType rtmp = QueryRangeImpl(2*treeIndex + 2, segMiddle + 1, segUpper, lower, upper, aditionalIndecies...);
            result = Traits.CombineResults(result, rtmp);
        }
        return result;
    }
    
    std::vector<TValue>& GetRaw() {
        return Tree;
    }
    
    const std::vector<TValue>& GetRaw() const {
        return Tree;
    }

private:
    int ElementsCount = 0; 
    std::vector<TValue> Tree;
    TTraits Traits;
};

template<typename TValue>
struct TSumTraits {
    using TQueryResultType = TValue;
    using TValueType = TValue;
    
    inline TValue Neutral() const {
        return {};
    }
    
    inline TValue CombineValues(const TValue& l, const TValue& r) const {
        return l + r;
    }
    
    inline TValue CombineResults(const TValue& l, const TValue& r) const {
        return l + r;
    }
    
    inline TValue ContinueQuery(const TValue& v) const {
        return v; 
    }
    
    inline void ContinueUpdate(TValue& v, const TValue& val) const {
        v = val; 
    }
};

template<typename TEnclosedTree>
struct TAddDimensionTraits {
    using TEnclosedTraits = typename TEnclosedTree::TTraitsType;
    using TQueryResultType = typename TEnclosedTraits::TQueryResultType;
    using TEnclosedValue = typename TEnclosedTraits::TValueType;
    
    inline TQueryResultType Neutral() const {
        return TQueryResultType{};
    }
    
    inline TEnclosedTree CombineValues(const TEnclosedTree& l, const TEnclosedTree& r) const {
        const auto& lraw = l.GetRaw();
        const auto& rraw = r.GetRaw();
        if (lraw.empty() || rraw.empty()) {
            return rraw.empty() ? l : r;
        }
        TEnclosedTree result = l;
        auto& raw = result.GetRaw();
        for (int i = 0; i < lraw.size(); ++i) {
            raw[i] = Traits.CombineValues(lraw[i], rraw[i]);
        }
        return result;
    }
    
    inline TQueryResultType CombineResults(const TQueryResultType& l, const TQueryResultType& r) const {
        return Traits.CombineResults(l, r);
    }
    
    template<typename ...TIndecies>
    inline TQueryResultType ContinueQuery(const TEnclosedTree& tree, TIndecies... aditionalIndecies) const {
        return tree.QueryRange(aditionalIndecies...); 
    }
    
    template<typename TFinalValue, typename ...TIndecies>
    inline void ContinueUpdate(TEnclosedTree& tree, const TFinalValue& val, TIndecies... aditionalIndecies) const {
        tree.Update(val, aditionalIndecies...);
    }
    
    TEnclosedTraits Traits;
};


using IntSumSegmentTree = TSegmentTree<int, TSumTraits<int>>;
using Int2DSumSegmentTree = TSegmentTree<IntSumSegmentTree, TAddDimensionTraits<IntSumSegmentTree>>;

class NumMatrix {
public:
    NumMatrix(vector<vector<int>>& matrix) 
        : Tree(BuildSegmnetTree(matrix))
    {}
    
    static Int2DSumSegmentTree BuildSegmnetTree(vector<vector<int>>& matrix) {
        std::vector<int> zeros(matrix.front().size());
        IntSumSegmentTree zeroTree(resizeInput(zeros));
        
        std::vector<IntSumSegmentTree> tree;
        tree.reserve(upper_power2(matrix.size()));
        for (auto& nums : matrix) {
            tree.emplace_back(resizeInput(nums));
        }
        tree.resize(upper_power2(tree.size()), zeroTree);
        return Int2DSumSegmentTree{std::move(tree)};
    }
    
    template<typename TValue>
    static vector<TValue>&& resizeInput(vector<TValue>& nums) {
        nums.resize(upper_power2(nums.size()), {});
        return std::move(nums);
    }
    
    static int upper_power2(int size) {
        int result = 1;
        while (result < size) {
            result <<= 1;
        }
        return result;
    }
    
    void update(int row, int col, int val) {
        Tree.Update(val, row, col);
    }
    
    int sumRegion(int row1, int col1, int row2, int col2) {
        return Tree.QueryRange(row1, row2, col1, col2);
    }
    
    Int2DSumSegmentTree Tree;
};

/**
 * Your NumMatrix object will be instantiated and called as such:
 * NumMatrix* obj = new NumMatrix(matrix);
 * obj->update(row,col,val);
 * int param_2 = obj->sumRegion(row1,col1,row2,col2);
 */