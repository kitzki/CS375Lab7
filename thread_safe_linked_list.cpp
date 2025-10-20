#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>

template <typename T>
class ThreadSafeLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr) {}
    };

    Node* head = nullptr;
    std::mutex mtx;

public:
    // Destructor to clean up nodes safely
    ~ThreadSafeLinkedList() {
        while (!empty()) {
            T value;
            pop_front(value);
        }
    }

    // Push element to the front
    void push_front(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
    }

    // Pop element from the front
    bool pop_front(T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!head) return false;
        Node* temp = head;
        value = temp->data;
        head = head->next;
        delete temp;
        return true;
    }

    // Check if the list is empty
    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return head == nullptr;
    }

    // Return list size
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        size_t count = 0;
        Node* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
};

// -------------------------------------------------
// Test Function
// -------------------------------------------------
void linkedListTest() {
    ThreadSafeLinkedList<int> list;
    std::vector<std::thread> threads;

    auto pusher = [&list](int id) {
        for (int i = 0; i < 5; ++i) {
            int value = id * 100 + i;
            list.push_front(value);
            std::cout << "Thread " << id << " pushed: " << value << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    auto popper = [&list]() {
        for (int i = 0; i < 10; ++i) {
            int value;
            if (list.pop_front(value)) {
                std::cout << "Popped: " << value << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    };

    // Launch threads
    threads.emplace_back(pusher, 1);
    threads.emplace_back(pusher, 2);
    threads.emplace_back(popper);

    // Wait for all threads to finish
    for (auto& t : threads)
        t.join();
}

int main() {
    linkedListTest();
    return 0;
}
