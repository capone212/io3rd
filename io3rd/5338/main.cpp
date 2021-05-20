#include <iostream>
#include <vector>
#include <unordered_map>
#include <optional>

struct SampleableSet {
public:
    void insert(int value) {
        auto index = ValuesVector.size();
        ValuesVector.push_back(value);
        Value2Index[value] = index;
    }

    void remove(int value) {
        auto it = Value2Index.find(value);
        if (it == Value2Index.end()) {
            return;
        }
        ValuesVector[it->second].reset();
        Value2Index.erase(it);
        reconcile();
    }

    bool contains(int value) const {
        return Value2Index.find(value) != Value2Index.end();
    }

    int sample() const {
        while (true)
        {
            int index = rand() %  ValuesVector.size();
            if (auto v = ValuesVector[index]) {
                return *v;
            }
        }
    }

    void reconcile() {
        if (ValuesVector.size() / 2 > Value2Index.size()) {
            return;
        }

        auto start = std::remove_if(ValuesVector.begin(), ValuesVector.end(), [](const auto& v) {
            return !v;
        });
        ValuesVector.erase(start, ValuesVector.end());
        for (int i = 0; i < ValuesVector.size(); ++i) {
            Value2Index[*ValuesVector[i]] = i;
        }
    }


private:
    std::unordered_map<int /*value */, std::size_t /*index*/> Value2Index;
    std::vector<std::optional<int>> ValuesVector;
};

int main() {
    
    return 0;
}
