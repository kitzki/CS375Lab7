#include <iostream>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>

template <typename T>
class ThreadSafeDeque {
private:
    std::deque<T> dq;
    std::mutex mtx;

public:
    // Push element at front
    void push_front(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        dq.push_front(value);
    }

    // Push element at back
    void push_back(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        dq.push_back(value);
    }

    // Pop element from front
    bool pop_front(T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (dq.empty()) return false;
        value = dq.front();
        dq.pop_front();
        return true;
    }

    // Pop element from back
    bool pop_back(T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (dq.empty()) return false;
        value = dq.back();
        dq.pop_back();
        return true;
    }

    // Check if deque is empty
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return dq.empty();
    }

    // Return deque size
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return dq.size();
    }
};

// -------------------------------------------
// Test Function
// -------------------------------------------
void dequeTest() {
    ThreadSafeDeque<int> dq;
    std::vector<std::thread> threads;

    auto push_front_task = [&dq](int id) {
        for (int i = 0; i < 5; ++i) {
            int value = id * 100 + i;
            dq.push_front(value);
            std::cout << "Thread " << id << " pushed front: " << value << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto push_back_task = [&dq](int id) {
        for (int i = 0; i < 5; ++i) {
            int value = id * 100 + i;
            dq.push_back(value);
            std::cout << "Thread " << id << " pushed back: " << value << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto pop_front_task = [&dq]() {
        for (int i = 0; i < 10; ++i) {
            int value;
            if (dq.pop_front(value)) {
                std::cout << "Popped front: " << value << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    auto pop_back_task = [&dq]() {
        for (int i = 0; i < 10; ++i) {
            int value;
            if (dq.pop_back(value)) {
                std::cout << "Popped back: " << value << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    // Launch threads
    threads.emplace_back(push_front_task, 1);
    threads.emplace_back(push_back_task, 2);
    threads.emplace_back(pop_front_task);
    threads.emplace_back(pop_back_task);

    // Join all threads
    for (auto& t : threads)
        t.join();
}

int main() {
    dequeTest();
    return 0;
}
