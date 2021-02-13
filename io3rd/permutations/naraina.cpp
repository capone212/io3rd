#include <iostream>
#include <vector>

using list_t = std::vector<int>;

void reflect(list_t& list, int start) {
    int i = start;
    int j = list.size() - 1;
    while (j > i) {
        std::swap(list[i], list[j]);
        ++i;
        --j;
    }
}


void next_permutation(list_t& list) {
    // Step 1: find max j, so a[j]<a[j+1]
    int max_j = list.size()-2;
    for ( ;max_j>0; max_j--) {
        if (list[max_j] < list[max_j+1]) {
            break;
        }
    }
    int max_l = list.size()-1;        
    // Step 2: find max l>max_j so a[l]>a[max_j], and swap them
    for (; max_l > max_j; max_l--) {
        if (list[max_l] > list[max_j]) {
            break;
        }
    } 

    std::swap(list[max_j], list[max_l]);
    reflect(list, max_j+1);
}

void print(const list_t& list) {
    for (const auto& l : list) {
        std::cout << l << "," ;
    }
    std::cout << std::endl;
}

int main() {
    list_t list = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    for (std::int64_t i = 0; i < 2ll*3*4*5*6*7*8*9*10*11*12; ++i) {
        // print(list);
        next_permutation(list);
    }
    // list_t list = {1, 2, 3, 4};
    // for (std::int64_t i = 0; i < 24; ++i) {
    //     print(list);
    //     next_permutation(list);
    // }
    return 0;
}