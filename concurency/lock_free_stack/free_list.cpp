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

template<typename TNode>
struct TNodeAllocator {
    TNodeAllocator() {
    };

    ~TNodeAllocator() {
        CleanupElements(Top.load(std::memory_order_relaxed));
    }

    using AtomicTaggedPointer = std::atomic<TaggedPointer<TNode>>;
    static_assert(AtomicTaggedPointer::is_always_lock_free, "Tagged Ptr is not lock free in this platform!");

    TaggedPointer<TNode> Alloc() {
        return MakeTaggedPointer(new TNode{}, ++AllocCounter); 
    }

    std::atomic_uint16_t AllocCounter = 0;    
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
            if (Parent.HazardScopes.load() == 1) {
                Stolen = Parent.Top.exchange({});
            }
            if (--Parent.HazardScopes == 0) {
                Parent.CleanupElements(Stolen);
                return;
            }
            // So we have to add them back
            Parent.RelinkExistingNodes(Stolen);
        }

        void Dealloc(TaggedPointer<TNode> node) {
            Parent.AddFreeList(node.Ptr());
        }

    private:
        TNodeAllocator& Parent;

        TaggedPointer<TNode> Stolen;
    };

private:
    void AddFreeList(TNode* node) {
        auto nodeToFree = MakeTaggedPointer(node, ++AllocCounter);
        nodeToFree.Ptr()->Next = Top.load(std::memory_order_relaxed);
        while (!Top.compare_exchange_weak(nodeToFree.Ptr()->Next, nodeToFree, std::memory_order_acquire, std::memory_order_relaxed)) {
        }
    }

    void CleanupElements(TaggedPointer<TNode> nodeToFree) {
        while (nodeToFree.Ptr()) {
            auto tmp = nodeToFree;
            nodeToFree = nodeToFree.Ptr()->Next;
            delete tmp.Ptr(); 
        }
    }

    void RelinkExistingNodes(TaggedPointer<TNode> head) {
        if (!head.Ptr()) {
            return;
        }
        // Get last node in the chain
        auto last = head;
        while (last.Ptr()->Next.Ptr()) {
            last = last.Ptr()->Next;
        }
        
        last.Ptr()->Next = Top.load(std::memory_order_relaxed);
        while (!Top.compare_exchange_weak(last.Ptr()->Next, head, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        }
    }

private:
    // Free list
    std::atomic<TaggedPointer<TNode>> Top;
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
        TNodeAllocator<TNode>::TScoppedCleanup scopedClean(Allocator);

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
        scopedClean.Dealloc(current);
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
    for (int i = 0; i < 12; ++i) {
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
