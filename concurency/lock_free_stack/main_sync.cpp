#include <atomic>
#include <optional>
#include <vector>
#include <thread>
#include <cassert>
#include <iostream>
#include <bitset>
#include <mutex>

const std::size_t CYCLES_COUNT = 1'000'000;
const std::size_t STACK_MAX_SIZE = 64 * 1024;


// Ready to be template parameter
// TODO: make cyclick buffer 
template<typename TNode>
struct TNodeAllocator {
    TNode* Alloc() {
        return new TNode;
    }

    void Dealloc(TNode* node) {
        // return;
        delete node;
    }
};


struct TStack {
    using TValue = int;

    bool Push(int value) {
        auto node = Allocator.Alloc();
        if (node == nullptr) {
            // stack is full
            return false;
        }

        node->Value = value;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            node->Next = Top;
            Top = node;
        }
        return true;
    }

    std::optional<TValue> Pop() {
        TNode* current = nullptr;
         {
            std::unique_lock<std::mutex> lock(m_mutex);
            current = Top;
            if (Top) {
                Top = Top->Next;
            }
        }
        if (!current) {
            return {};
        }
        auto value = current->Value;
        Allocator.Dealloc(current);
        return value;
    }


public:
    struct TNode {
        TValue Value = TValue();
        TNode* Next;
    };

private:
    TNode* Top = nullptr;
    TNodeAllocator<TNode> Allocator;
    std::mutex m_mutex;
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
