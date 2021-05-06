
#include <numeric>

class Solution {
public:
    using iterator_t = std::vector<int>::const_iterator;

    int trap(const std::vector<int>& height) {
        auto it = height.begin();
        const auto end = height.end();
        int current = 0;
        while (it != end) {
            auto next_wall = find_next_wall(it, end);
            current += count_water(it, next_wall, end);
            it = next_wall;
        }
        return current;
    }

    inline int count_water(iterator_t start, iterator_t finish, iterator_t end) {
        int result = count_capacity(start, finish, end) - count_bricks(start, finish, end);
        assert(result >= 0);
        return result;
    }

    inline int count_capacity(iterator_t start, iterator_t finish, iterator_t end) {
        if (start == end || finish == end || start == finish) {
            return 0;
        }
        return (std::distance(start, finish) - 1) * std::min(*start, *finish);
    }

    inline int count_bricks(iterator_t start, iterator_t finish, iterator_t end) {
        if (start == end || finish == end || start == finish) {
            return 0;
        }
        start = std::next(start);
        return std::accumulate(start, finish, 0);
    }

    iterator_t find_next_wall(iterator_t begin, iterator_t end) {
        const int value = *begin;
        auto best_it = end;
        for (auto it = std::next(begin); it != end; ++it) {
            if (*it >= value) {
                return it;
            }
            if (best_it == end || *it > *best_it) {
                best_it = it; 
            } 
        }
        return best_it;
    }

}; // class
