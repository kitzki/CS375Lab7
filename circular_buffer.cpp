#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>

#define BUFFER_SIZE 5

// ==========================================
// Thread-Safe Circular Buffer
// ==========================================
class ThreadSafeCircularBuffer {
private:
    int buffer[BUFFER_SIZE];
    int in = 0, out = 0, count = 0;
    std::mutex mtx;
    std::condition_variable not_full, not_empty;

public:
    // Push a value into the buffer (waits if full)
    void push(int value) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return count < BUFFER_SIZE; });  // Wait if buffer full

        buffer[in] = value;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        std::cout << "Buffer PUSH -> " << value << " | count=" << count << std::endl;

        lock.unlock();
        not_empty.notify_one();  // Notify one waiting consumer
    }

    // Pop a value from the buffer (waits if empty)
    bool pop(int& value) {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return count > 0; });  // Wait if buffer empty

        value = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        std::cout << "Buffer POP <- " << value << " | count=" << count << std::endl;

        lock.unlock();
        not_full.notify_one();  // Notify one waiting producer
        return true;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return count == 0;
    }

    bool full() {
        std::lock_guard<std::mutex> lock(mtx);
        return count == BUFFER_SIZE;
    }
};

// ==========================================
// Test with Multiple Producers and Consumers
// ==========================================
void circularBufferTest() {
    ThreadSafeCircularBuffer cb;
    std::vector<std::thread> producers, consumers;
    const int NUM_ITEMS = 10;
    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 2;

    auto producer = [&](int id) {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            int value = rand() % 100;
            cb.push(value);
            std::cout << "Producer " << id << " produced: " << value << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto consumer = [&](int id) {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            int value;
            cb.pop(value);
            std::cout << "Consumer " << id << " consumed: " << value << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    // Start producer and consumer threads
    for (int i = 0; i < NUM_PRODUCERS; ++i)
        producers.emplace_back(producer, i);

    for (int i = 0; i < NUM_CONSUMERS; ++i)
        consumers.emplace_back(consumer, i);

    // Join all threads
    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    std::cout << "\nAll threads finished.\n";
}

// ==========================================
// Main Function
// ==========================================
int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::cout << "=== Thread-Safe Circular Buffer Test ===\n";
    circularBufferTest();
    return 0;
}
