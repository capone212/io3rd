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
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            Stack.push_back(value);
        }
        return true;
    }

    std::optional<TValue> Pop() {
        TValue val;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (Stack.empty()) {
                return {};
            }
            val = Stack.back();
            Stack.pop_back();
        }
        return val;
    }


public:
    struct TNode {
        TValue Value = TValue();
        TNode* Next;
    };

private:
    std::vector<int> Stack;
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
