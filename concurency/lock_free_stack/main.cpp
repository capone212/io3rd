#include <atomic>
#include <iostream>
#include <optional>
#include <vector>
#include <thread>
#include <cassert>

const std::size_t CYCLES_COUNT = 1'000'000;
const std::size_t STACK_MAX_SIZE = 64 * 1024; 

// Ready to be template parameter
// TODO: make cyclick buffer 
template<typename TNode>
struct TNodeAllocator {
    TNodeAllocator() {
        for (int i = 0; i < STACK_MAX_SIZE; ++i) {
            CycleBuffer.push_back(std::make_unique<TNode>());
        }
    };

    TNode* Alloc() {
        // Attempt stack full size at max
        for (int i = 0; i < STACK_MAX_SIZE; ++i) {
            auto index = Counter++ %  STACK_MAX_SIZE;
            auto& node = CycleBuffer[index];
            bool oldValue = false;
            if (node->Allocated.compare_exchange_strong(oldValue, true, std::memory_order_relaxed) && oldValue == false) {
                return node.get(); 
            }
        }
        // Can't find free nodes 
        return nullptr;
    }

    void Dealloc(TNode* node) {
        assert(node->Allocated);
        node->Allocated = false;
    }
    using TNodePtr = std::unique_ptr<TNode>;
    std::vector<TNodePtr> CycleBuffer;
    std::atomic_uint32_t Counter = 0;
};


struct TStack {
    using TValue = int;

    bool Push(int value) {
        auto* node = Allocator.Alloc();
        if (!node) {
            // stack is full
            return false;
        }
        node->Value = value;
        node->Next = Top.load(std::memory_order_relaxed);
        while (!Top.compare_exchange_weak(node->Next, node, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        }
        return true;
    }

    std::optional<TValue> Pop() {
        auto* current = Top.load(std::memory_order_relaxed);
        if (current == nullptr) {
            return {};
        }
        while (!Top.compare_exchange_weak(current, current->Next, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            if (current == nullptr) {
                return {};
            } 
        }
        auto value = current->Value;
        Allocator.Dealloc(current);
        return value;
    }


public:
    struct TNode {
        TValue Value = TValue();
        TNode* Next = nullptr;
        std::atomic_bool Allocated = false;
    };

private:
    std::atomic<TNode*> Top;
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
