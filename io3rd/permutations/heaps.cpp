#include <iostream>
#include <vector>


using list_t = std::vector<int>;

void print(const list_t& list) {
    for (const auto& l : list) {
        std::cout << l << "," ;
    }
    std::cout << std::endl;
}

void permute(list_t& list, int subn, int64_t& count) {
    if (subn==1) {
        print(list);
        ++count;
        return;
    }
    permute(list, subn-1, count);
    for (int i = 0; i < subn-1; ++i) {
        if (i % 2 == 0) {
            std::swap(list[i], list[subn-1]);
        } else {
            std::swap(list[0], list[subn-1]);
        }
        permute(list, subn-1, count);
    }
}

int main() {
    // list_t list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    list_t list = {1, 2, 3, 4};
    int64_t count = 0;
    permute(list, list.size(), count);
    std::cout << "Count: " << count << std::endl;
    return 0;
}