#include <iostream>
#include "tagged_ptr.h"
#include <memory>

struct Shit {
    int x = 23;
};

int main() {
    auto heapShit = std::make_unique<Shit>();
    TaggedPointer<Shit> tptr;
    tptr.Value = reinterpret_cast<uintptr_t>(heapShit.get());
    tptr.Ptr()->x = 64;
    std::cout << heapShit->x << std::endl ;

    auto tptr2 = MakeTaggedPointer(heapShit.get(), 3654);
    if (tptr2.Ptr() != heapShit.get()) {
        throw std::runtime_error("ptr does not match!");
    }
    if (tptr2.Tag() != 3654) {
        throw std::runtime_error("tag does not match!");
    }
    return 0;
}