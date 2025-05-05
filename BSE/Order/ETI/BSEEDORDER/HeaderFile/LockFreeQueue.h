#include <atomic>
#include <memory>
#include <utility> // For std::pair
#include <condition_variable>
#include <mutex>

template <typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        Node(T const& data_) : data(data_), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::condition_variable cv;
    std::mutex cv_m;

public:
    LockFreeQueue() {
        Node* node = new Node(T()); // dummy node
        head.store(node);
        tail.store(node);
    }

    ~LockFreeQueue() {
        while (Node* node = head.load()) {
            head.store(node->next.load());
            delete node;
        }
    }

    void push(T value) {
        Node* newNode = new Node(value);
        Node* oldTail = tail.exchange(newNode);
        oldTail->next.store(newNode);

        {
            std::lock_guard<std::mutex> lk(cv_m);
            cv.notify_one(); // Notify the consumer thread
        }
    }

    bool try_pop(T& value) {
        Node* oldHead = head.load();
        Node* newHead = oldHead->next.load();
        if (oldHead == tail.load()) {
            if (newHead == nullptr)
                return false;
            tail.compare_exchange_strong(oldHead, newHead);
        }
        else {
            value = newHead->data;
            head.store(newHead);
            delete oldHead;
            return true;
        }
        return false;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [this] { return head.load() != tail.load(); });
        try_pop(value);
    }

    bool empty() const {
        return (head.load() == tail.load());
    }
};
