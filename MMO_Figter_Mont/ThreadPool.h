#include <iostream>
#include <Windows.h>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>

//#include <atomic>
//#include <functional>
//#include <queue>
//#include <thread>
//#include <vector>

// ������ Ǯ Ŭ���� ����
class ThreadPool {
public:
    // ������
    ThreadPool(size_t numThreads) : stop(false) {
        hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // �̺�Ʈ ��ü ����
        for (size_t i = 0; i < numThreads; ++i) {
            // �����带 �����ϰ� �����մϴ�.
            threads.push_back(CreateThread(NULL, 0, &ThreadPool::ThreadMain, this, 0, NULL));
        }
    }

    //// ��� �Լ��� ������ Ǯ�� �߰��ϴ� �Լ�
    //template<typename T, typename... Args>
    //void Enqueue(void (T::* memberFunction)(Args...), T* obj, Args... args) {
    //    {
    //        std::unique_lock<std::mutex> lock(queueMutex);
    //        // �۾��� ť�� �߰��մϴ�.
    //        tasks.emplace([=]() { std::invoke(memberFunction, obj, args...); });
    //    }
    //    // �۾��� �߰��Ǿ����� �˸��� ���� �̺�Ʈ�� �ñ׳��մϴ�.
    //    SetEvent(hEvent);
    //}
    template<typename T>
    void Enqueue(void (T::* memberFunction)(stCapturedPacket), T* obj, stCapturedPacket packet) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // �۾��� ť�� �߰��մϴ�.
            tasks.emplace([=, func = std::move(memberFunction), o = std::move(obj), p = std::move(packet)]() {
                std::invoke(func, o, p); // packet�� �״�� ����
                });
        }
        // �۾��� �߰��Ǿ����� �˸��� ���� �̺�Ʈ�� �ñ׳��մϴ�.
        SetEvent(hEvent);
    }


    // �Ҹ���
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // ������ Ǯ ���� �÷��׸� �����մϴ�.
            stop = true;
        }
        // ��� �����忡�� ���Ḧ �˸��� ���� �̺�Ʈ�� �ñ׳��մϴ�.
        SetEvent(hEvent);
        // ��� �����尡 ����� ������ ����մϴ�.
        WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);
        // �̺�Ʈ ��ü�� �ݽ��ϴ�.
        CloseHandle(hEvent);
    }

private:
    // ������ �ڵ��� �����ϴ� ����
    std::vector<HANDLE> threads;
    // �۾��� �����ϴ� ť
    std::queue<std::function<void()>> tasks;
    // ť�� �����ϱ� ���� ���ؽ�
    std::mutex queueMutex;
    // ������ Ǯ ���� �÷���
    bool stop;
    // �̺�Ʈ �ڵ�
    HANDLE hEvent;

    // �����尡 �����ϴ� ���� �Լ�
    static DWORD WINAPI ThreadMain(LPVOID lpParam) {
        ThreadPool* pool = static_cast<ThreadPool*>(lpParam);
        while (true) {
            DWORD dwWaitResult = WaitForSingleObject(pool->hEvent, INFINITE);
            if (dwWaitResult == WAIT_OBJECT_0) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(pool->queueMutex);
                    if (!pool->tasks.empty()) {
                        task = pool->tasks.front();
                        pool->tasks.pop();
                    }
                    else {
                        continue;
                    }
                }
                // �Լ��� ȣ���մϴ�.
                task();
            }
            else {
                break;
            }
        }
        return 0;
    }
};

/***************/
/* ��-���� ���*/
/***************/
/*
class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&ThreadPool::ThreadMain, this);
        }
    }

    template<typename T>
    void Enqueue(void (T::* memberFunction)(stCapturedPacket), T* obj, stCapturedPacket packet) {
        tasks.push([=, func = std::move(memberFunction), o = std::move(obj), p = std::move(packet)]() {
            std::invoke(func, o, p);
            });
    }

    ~ThreadPool() {
        stop = true;
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    std::vector<std::thread> threads;
    std::atomic<bool> stop;

    // Lock-Free Queue Implementation
    template<typename T>
    class LockFreeQueue {
    public:
        LockFreeQueue() : head(new Node), tail(head.load()) {}

        ~LockFreeQueue() {
            T output;
            while (pop(output));
            delete head.load();
        }

        void push(const T& value) {
            Node* newNode = new Node(value);
            Node* tailSnapshot = tail.load();
            while (true) {
                Node* next = tailSnapshot->next.load();
                if (!next) {
                    if (tailSnapshot->next.compare_exchange_weak(next, newNode)) {
                        break;
                    }
                }
                else {
                    tail.compare_exchange_weak(tailSnapshot, next);
                    tailSnapshot = tail.load();
                }
            }
            tail.compare_exchange_weak(tailSnapshot, newNode);
        }

        bool pop(T& value) {
            Node* headSnapshot = head.load();
            while (true) {
                Node* next = headSnapshot->next.load();
                if (!next) {
                    return false;
                }
                if (head.compare_exchange_weak(headSnapshot, next)) {
                    value = next->data;
                    delete headSnapshot;
                    return true;
                }
            }
        }

    private:
        struct Node {
            T data;
            std::atomic<Node*> next;

            Node() : next(nullptr) {}
            Node(const T& value) : data(value), next(nullptr) {}
        };

        std::atomic<Node*> head;
        std::atomic<Node*> tail;
    };

    LockFreeQueue<std::function<void()>> tasks;

    void ThreadMain() {
        while (!stop) {
            std::function<void()> task;
            if (tasks.pop(task)) {
                task();
            }
            else {
                std::this_thread::yield();
            }
        }
    }
};
*/

/***************/
/* ����-�� ���*/
/***************/
/*
class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&ThreadPool::ThreadMain, this);
        }
    }

    template<typename T>
    void Enqueue(void (T::* memberFunction)(stCapturedPacket), T* obj, stCapturedPacket packet) {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.emplace([=, func = std::move(memberFunction), o = std::move(obj), p = std::move(packet)]() {
            std::invoke(func, o, p);
            });
    }

    ~ThreadPool() {
        stop = true;
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::atomic<bool> stop;

    void ThreadMain() {
        while (!stop) {
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!tasks.empty()) {
                    task = std::move(tasks.front());
                    tasks.pop();
                }
            }
            if (task) {
                task();
            }
            else {
                std::this_thread::yield();
            }
        }
    }
};
*/