#include <atomic>
#include <optional>
#include <vector>
#include <thread>
#include <cassert>
#include <iostream>
#include <bitset>

#include "tagged_ptr.h"

const std::size_t CYCLES_COUNT = 1'000'000;
const std::size_t STACK_MAX_SIZE = 1024;


// Ready to be template parameter
// TODO: make cyclick buffer 
template<typename TNode>
struct TNodeAllocator {
    TNodeAllocator() {
        for (int i = 0; i < STACK_MAX_SIZE; ++i) {
            CycleBuffer.push_back(std::make_unique<TNode>());
        }
    };

    using AtomicTaggedPointer = std::atomic<TaggedPointer<TNode>>;
    static_assert(AtomicTaggedPointer::is_always_lock_free, "Tagged Ptr is not lock free in this platform!");

    TaggedPointer<TNode> Alloc() {
        // return new TNode{};

        // Attempt stack full size at max
        for (int i = 0; i < STACK_MAX_SIZE; ++i) {
            auto index = IndexCounter++ %  STACK_MAX_SIZE;
            auto& node = CycleBuffer[index];
            bool oldValue = false;
            if (node->Allocated.compare_exchange_strong(oldValue, true) && oldValue == false) {
                return MakeTaggedPointer(node.get(), ++node->Tag); 
            }
        }
        // Can't find free nodes 
        return {};
    }

    void Dealloc(TaggedPointer<TNode> node) {
        // return;
        assert(node.Ptr()->Allocated);
        node.Ptr()->Allocated = false;
    }
    using TNodePtr = std::unique_ptr<TNode>;
    std::vector<TNodePtr> CycleBuffer;
    std::atomic_uint32_t IndexCounter = 0;
    std::atomic_uint16_t AllocCounter = 0;
};


struct TStack {
    using TValue = int;

    bool Push(int value) {
        auto node = Allocator.Alloc();
        if (node.Ptr() == nullptr) {
            // stack is full
            return false;
        }
        node.Ptr()->Value = value;
        node.Ptr()->Next = Top.load(std::memory_order_relaxed);
        while (!Top.compare_exchange_weak(node.Ptr()->Next, node, std::memory_order_acquire, std::memory_order_relaxed)) {
        }
        return true;
    }

    std::optional<TValue> Pop() {
        auto current = Top.load(std::memory_order_relaxed);
        if (current.Ptr() == nullptr) {
            return {};
        }
        while (!Top.compare_exchange_weak(current, current.Ptr()->Next, std::memory_order_acquire, std::memory_order_relaxed)) {
            if (current.Ptr() == nullptr) {
                return {};
            } 
        }
        auto value = current.Ptr()->Value;
        Allocator.Dealloc(current);
        return value;
    }


public:
    struct TNode {
        TValue Value = TValue();
        TaggedPointer<TNode> Next;
        std::atomic_bool Allocated = false;
        std::uint16_t Tag = 0;
    };

private:
    std::atomic<TaggedPointer<TNode>> Top;
    TNodeAllocator<TNode> Allocator;
};


TStack TheStack;

void DoManyPush() {
    int count = 0;
    while (count < CYCLES_COUNT) {
        if (!TheStack.Push(212)) {
            // std::cout << "Can't push value. Stack seems to be full" << std::endl;
        } else {
            ++count;
        }
    }
}

void DoManyPop() {
    int count = 0;
    while (count < CYCLES_COUNT) {
        if (auto value = TheStack.Pop(); !value) {
            // std::cout << "Can't pop value. Stack seems to be empty" << std::endl;
        } else {
            ++count;
        }
    }
}

int main() {
    // Test single thread
    if (!TheStack.Push(10)) {
        throw std::runtime_error("Can't push");
    }
    if (auto value = TheStack.Pop(); !value || *value != 10) {
        throw std::runtime_error("Wrong pop!");
    }
    // Test multithread
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) {
        if (i % 2 == 0) {
            threads.emplace_back(&DoManyPop);
        } else {
            threads.emplace_back(&DoManyPush);
        }
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
