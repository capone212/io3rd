#include <atomic>
#include <iostream>
#include <thread>
#include <future>

// std::atomic<bool> x = false;
// std::atomic<bool> y = false;
std::atomic_int z = 0;

int a = 0;
int b = 0;

int handler1() {
    b = 1;
    return a;
}

int handler2() {
    a = 1;
    return b;
}

auto run_task(auto handler) {
    std::packaged_task<int()> task(handler);
    std::future<int> f1 = task.get_future();  // get a future
    std::thread t1(std::move(task)); // launch on a thread
    t1.detach();
    return f1;
}


int main() {

    while (true) {
        a = 0;
        b = 0;
        std::atomic_thread_fence(std::memory_order_acq_rel);;
        auto f1 = run_task(&handler1);
        auto f2 = run_task(&handler2);
        int lx = f1.get();
        int ly = f2.get();
        std::cout << "x1:" << lx << " x2:" << ly << std::endl;
        if (lx == ly && lx == 0) {
            break;
        }
    }
    return 0;
}
