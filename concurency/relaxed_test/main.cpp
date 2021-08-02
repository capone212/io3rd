#include <atomic>
#include <iostream>
#include <thread>

std::atomic<bool> x = false;
std::atomic<bool> y = false;
std::atomic_int z = 0;

void handler1() {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
}

void handler2() {
    while (!y.load(std::memory_order_relaxed)) {
    }
    if (x.load(std::memory_order_relaxed)) {
        ++z;
    }
}

int main() {
    std::thread t1(&handler2);
    std::thread t2(&handler1);
    t1.join();
    t2.join();
    std::cout << z << std::endl;
    return 0;
}
