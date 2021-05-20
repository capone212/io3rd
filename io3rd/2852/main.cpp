#include <iostream>
#include <vector>
#include <mutex>

using handler_t = std::function<void()>;

struct CallbackList {
    void subscribe(handler_t handler) {
        std::unique_lock<std::mutex> lock(Guard);
        Handlers.push_back(handler);
    }

    void Fire() {
        if (InProgress++ != 0) {
            // in progress
            return;
        }
        do {
            std::vector<handler_t> handlers;  
            {
                std::unique_lock<std::mutex> lock(Guard);
                handlers.swap(Handlers);
                
            }
            for (auto handler : handlers) {
                handler();
            }
        } while (--InProgress != 0)
    }
private:
    std::vector<handler_t> Handlers;
    std::mutex Guard;
    std::atomic_int32 InProgress;
}