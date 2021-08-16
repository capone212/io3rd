#include <atomic>
#include <optional>
#include <vector>
#include <thread>
#include <cassert>
#include <iostream>
#include <bitset>

const std::size_t CYCLES_COUNT = 1'000'000;
constexpr static std::size_t MAX_THREAD_COUNT = 64;

template<typename TNode>
struct THazardStore {

    using THazardPtr = std::atomic<TNode*>;

    THazardPtr& GetHazardPtrForThread() {
        static thread_local TStoreClient threadClient(*this);
        return threadClient.HazardPtr();
    }

    bool HasHazardPtrFor(TNode* ptr) {
        for (auto& rec : Store) {
            if (rec.HPtr.load(std::memory_order_relaxed) == ptr) {
                return true;
            }
        }
        return false;
    }

private:
    struct TStoreClient{
        TStoreClient(THazardStore& parent) {
            Rec = [&] () {
                for (auto& r : parent.Store) {
                    std::thread::id emptyId;
                    if (r.Owner.compare_exchange_strong(emptyId, std::this_thread::get_id(), std::memory_order_relaxed)) {
                        return &r;
                    }
                }
                throw std::runtime_error("Can't allocate hazard pointer for thread!");
            }();
        }

        THazardPtr& HazardPtr() {
            return Rec->HPtr;
        }

        ~TStoreClient() {
            Rec->Owner.store(std::thread::id(), std::memory_order_relaxed);
        }

    private:
        THazardStore::TRec* Rec = nullptr;
    };

    struct TRec{
        std::atomic<TNode*> HPtr = nullptr;
        std::atomic<std::thread::id> Owner;
    };

private:
    std::array<TRec, MAX_THREAD_COUNT> Store;
};

template<typename TNode>
struct TFreeList {
    ~TFreeList() {
        for (const auto* node :  List) {
            delete node;
        }
    }

    void PushNode(TNode* node) {
        List.push_back(node);
    }

    template<typename TFunctor>
    void FilterNodes(TFunctor filter) {
        std::vector<TNode*> newList;
        newList.reserve(List.size());
        for (auto* node :  List) {
            if (!filter(node)) {
                newList.push_back(node);
            }
        }
        newList.swap(List);
    }

    inline std::size_t Size() {
        return List.size();
    }

    static TFreeList<TNode>& Instance() {
        static thread_local TFreeList<TNode> Instance;
        return Instance;
    }

private:
    std::vector<TNode*> List;;
};

struct TStack {
    using TValue = int;

    ~TStack() {
    }

    bool Push(int value) {
        auto node = new TNode{};
        node->Value = value;
        node->Next = Top.load(std::memory_order_relaxed);
        while (!Top.compare_exchange_weak(node->Next, node, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        }
        return true;
    }

    struct TNode {
        TValue Value = TValue();
        TNode* Next;
    };
    using ToFreeList = TFreeList<TNode>;

    std::optional<TValue> Pop() {
        auto& hazardPtr = HazardStore.GetHazardPtrForThread();
        TNode* current = Top.load(std::memory_order_relaxed);
        do {
            TNode* tmp = nullptr;
            // Load current top and mark it in use with hazard pointer
            do {
                tmp = current;
                hazardPtr.store(current, std::memory_order_acq_rel);
                current = Top.load(std::memory_order_relaxed);
            } while(tmp != current);
            if (current == nullptr) {
                return {};
            }
        } while (!Top.compare_exchange_weak(current, current->Next, std::memory_order_acq_rel, std::memory_order_relaxed));
        
        auto value = current->Value;
        hazardPtr.store(nullptr, std::memory_order_release);
        
        if (!HazardStore.HasHazardPtrFor(current)) {
            delete current;
        } else  {
            ToFreeList::Instance().PushNode(current);
            std::size_t CLEANUP_SIZE = 64;
            if (ToFreeList::Instance().Size()) {
                CleanupOrphants();
            }
        }
        return value;
    }

private:
    void CleanupOrphants() {
        std::vector<TNode*> nodesToFree;
        ToFreeList::Instance().FilterNodes([&](TNode* node){
            if (HazardStore.HasHazardPtrFor(node) == 0) {
                nodesToFree.push_back(node);
                return true;
            }
            return false;
        });
        for (auto* node : nodesToFree) {
            delete node;
        }
    }

private:
    std::atomic<TNode*> Top;
    THazardStore<TNode> HazardStore;
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
