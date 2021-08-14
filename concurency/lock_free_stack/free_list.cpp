#include <atomic>
#include <optional>
#include <vector>
#include <thread>
#include <cassert>
#include <iostream>
#include <bitset>

#include "tagged_ptr.h"

const std::size_t CYCLES_COUNT = 1'000'000;
const std::size_t STACK_MAX_SIZE = 1 * 1024;


// Ready to be template parameter
// TODO: make cyclick buffer 

template<typename TNode>
struct TNodeAllocator {
    TNodeAllocator() {
    };

    struct TFreeListNode {
        std::unique_ptr<TNode> Payload;
        TaggedPointer<TFreeListNode> Next;
    };

    using AtomicTaggedPointer = std::atomic<TaggedPointer<TNode>>;
    static_assert(AtomicTaggedPointer::is_always_lock_free, "Tagged Ptr is not lock free in this platform!");

    TaggedPointer<TNode> Alloc() {
        return MakeTaggedPointer(new TNode{}, ++AllocCounter); 
    }

    void Dealloc(TaggedPointer<TNode> node) {
        AddFreeList(node.Ptr());
    }

    void AddFreeList(TNode* node) {
        auto nodeToFree = MakeTaggedPointer(new TFreeListNode{}, ++DeallocCounter);
        nodeToFree.Ptr()->Payload.reset(node);
        nodeToFree.Ptr()->Next=Top.load();
        while (!Top.compare_exchange_weak(nodeToFree.Ptr()->Next, nodeToFree, std::memory_order_acquire, std::memory_order_relaxed)) {
        }
    }

    void CleanupElements(TaggedPointer<TFreeListNode> nodeToFree) {
        while (nodeToFree.Ptr) {
            auto tmp = nodeToFree;
            nodeToFree = nodeToFree.Ptr()->Next;
            delete tmp.Ptr(); 
        }
    }

    std::atomic_uint16_t AllocCounter = 0;
    std::atomic_uint16_t DeallocCounter = 0;
    
    std::atomic_uint16_t HazardScopes = 0;

    struct TScoppedCleanup {
        TScoppedCleanup( const TScoppedCleanup& ) = delete; // non construction-copyable
        TScoppedCleanup& operator=( const TScoppedCleanup& ) = delete; // non copyable

        TScoppedCleanup(TNodeAllocator& parent)
            : Parent(parent)
        {
            ++Parent.HazardScopes;
        }

        ~TScoppedCleanup() {
            // Steal elements from free list
            Stolen = Parent.Top.exchange({});
            if (--Parent.HazardScopes == 0) {
                CleanupElements(Stolen);
                return;
            }
            // So we have to add them back
            while (Stolen.Ptr) {
                auto tmp = Stolen;
                Stolen = Stolen.Ptr()->Next;
                Parent.AddFreeList(tmp.Ptr()->Payload.release());
                delete tmp.Ptr(); 
            }
        } 

        TNodeAllocator& Parent;

        TaggedPointer<TFreeListNode> Stolen;
    };

    // Free list
    std::atomic<TaggedPointer<TFreeListNode>> Top;
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
    for (int i = 0; i < 32; ++i) {
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
