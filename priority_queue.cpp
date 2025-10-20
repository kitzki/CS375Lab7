#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>

// ============================================
// Thread-Safe Priority Queue Class
// ============================================
template <typename T>
class ThreadSafePriorityQueue {
private:
    std::priority_queue<T> pq;
    std::mutex mtx;

public:
    ThreadSafePriorityQueue() {}

    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        pq.push(value);
    }

    bool pop(T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (pq.empty()) return false;
        value = pq.top();
        pq.pop();
        return true;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return pq.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return pq.size();
    }
};

// ============================================
// Test: Multiple Threads Pushing and Popping
// ============================================
void priorityQueueTest() {
    ThreadSafePriorityQueue<int> pq;
    std::vector<std::thread> threads;
    const int NUM_PUSHER_THREADS = 4;
    const int ITEMS_PER_THREAD = 5;

    auto pusher = [&](int id) {
        for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
            int priority = rand() % 100;  // Random priority value
            pq.push(priority);
            std::cout << "Thread " << id << " pushed: " << priority << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto popper = [&]() {
        for (int i = 0; i < NUM_PUSHER_THREADS * ITEMS_PER_THREAD; ++i) {
            int value;
            if (pq.pop(value)) {
                std::cout << "Popped: " << value << "\n";
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
        }
    };

    // Create pusher threads
    for (int i = 0; i < NUM_PUSHER_THREADS; ++i) {
        threads.emplace_back(pusher, i);
    }

    // Create one popper thread
    threads.emplace_back(popper);

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nAll threads finished. Remaining items in queue: " << pq.size() << std::endl;
}

// ============================================
// Main Function
// ============================================
int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::cout << "=== Thread-Safe Priority Queue Test ===\n";
    priorityQueueTest();
    return 0;
}
