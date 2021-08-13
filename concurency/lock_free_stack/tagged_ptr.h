#pragma once

#include <atomic>
#include <cstdint>

static const uintptr_t MASK_PTR = ~((~std::uintptr_t{0}) << 48); 

template<typename TType>
struct TaggedPointer {
    uintptr_t Value = 0;

    std::uint16_t Tag() const {
        return (Value >> 48);
    }
    TType* Ptr() {
        return reinterpret_cast<TType*>(MASK_PTR & Value);
    }
};

template<typename T>
TaggedPointer<T> MakeTaggedPointer(T* ptr, std::uint16_t tag) {
    TaggedPointer<T> result;
    result.Value = tag;
    result.Value <<= 48;
    result.Value |= reinterpret_cast<uintptr_t>(ptr);
    return result;
}


