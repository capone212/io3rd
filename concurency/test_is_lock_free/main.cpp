#include <atomic>
#include <cstdint>

struct TNode {

};


struct TTagedPointer {
    int Tag = 0;
    int PtrHigh = 0;
    // int PtrLow = 0;
};

// static_assert(sizeof(TTagedPointer) == 12, "Wrong size");


int main() {

    static_assert(std::atomic<TTagedPointer>::is_always_lock_free, "is not lock free");
    return 0;
}