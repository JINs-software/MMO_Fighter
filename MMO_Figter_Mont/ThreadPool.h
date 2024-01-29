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

// 스레드 풀 클래스 정의
class ThreadPool {
public:
    // 생성자
    ThreadPool(size_t numThreads) : stop(false) {
        hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // 이벤트 객체 생성
        for (size_t i = 0; i < numThreads; ++i) {
            // 스레드를 생성하고 시작합니다.
            threads.push_back(CreateThread(NULL, 0, &ThreadPool::ThreadMain, this, 0, NULL));
        }
    }

    //// 멤버 함수를 스레드 풀에 추가하는 함수
    //template<typename T, typename... Args>
    //void Enqueue(void (T::* memberFunction)(Args...), T* obj, Args... args) {
    //    {
    //        std::unique_lock<std::mutex> lock(queueMutex);
    //        // 작업을 큐에 추가합니다.
    //        tasks.emplace([=]() { std::invoke(memberFunction, obj, args...); });
    //    }
    //    // 작업이 추가되었음을 알리기 위해 이벤트를 시그널합니다.
    //    SetEvent(hEvent);
    //}
    template<typename T>
    void Enqueue(void (T::* memberFunction)(stCapturedPacket), T* obj, stCapturedPacket packet) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // 작업을 큐에 추가합니다.
            tasks.emplace([=, func = std::move(memberFunction), o = std::move(obj), p = std::move(packet)]() {
                std::invoke(func, o, p); // packet을 그대로 전달
                });
        }
        // 작업이 추가되었음을 알리기 위해 이벤트를 시그널합니다.
        SetEvent(hEvent);
    }


    // 소멸자
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // 스레드 풀 종료 플래그를 설정합니다.
            stop = true;
        }
        // 모든 스레드에게 종료를 알리기 위해 이벤트를 시그널합니다.
        SetEvent(hEvent);
        // 모든 스레드가 종료될 때까지 대기합니다.
        WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);
        // 이벤트 객체를 닫습니다.
        CloseHandle(hEvent);
    }

private:
    // 스레드 핸들을 저장하는 벡터
    std::vector<HANDLE> threads;
    // 작업을 저장하는 큐
    std::queue<std::function<void()>> tasks;
    // 큐에 접근하기 위한 뮤텍스
    std::mutex queueMutex;
    // 스레드 풀 종료 플래그
    bool stop;
    // 이벤트 핸들
    HANDLE hEvent;

    // 스레드가 실행하는 메인 함수
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
                // 함수를 호출합니다.
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
/* 락-프리 방식*/
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
/* 스핀-락 방식*/
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