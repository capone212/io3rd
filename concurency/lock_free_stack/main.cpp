#include <atomic>
#include <iostream>
#include <optional>
#include <vector>
#include <thread>

TStack TheStack;
const std::size_t CYCLES_COUNT = 1'000'000;

// Ready to be template parameter
template<typename TNode>
struct TNodeAllocator {
    TNode* Alloc() {
        return new TNode{};
    }

    void Dealloc(TNode* node) {
        // delete node;
    }

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


private:
    struct TNode {
        TValue Value = TValue();
        TNode* Next = nullptr;
    };

    std::atomic<TNode*> Top;
    TNodeAllocator<TNode> Allocator;
};

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
