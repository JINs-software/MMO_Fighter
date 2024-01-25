#include <iostream>
#include <Windows.h>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>

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

    // ��� �Լ��� ������ Ǯ�� �߰��ϴ� �Լ�
    template<typename T, typename... Args>
    void Enqueue(void (T::* memberFunction)(Args...), T* obj, Args... args) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // �۾��� ť�� �߰��մϴ�.
            tasks.emplace([=]() { std::invoke(memberFunction, obj, args...); });
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

// ���� Ŭ���� ����
class Example {
public:
    // ��� �Լ�
    void MemberFunction(int i, double d) {
        std::cout << "Member function called with: " << i << " and " << d << std::endl;
    }
};

//int main() {
//    // ���� ��ü ����
//    Example example;
//    // ������ Ǯ ����
//    ThreadPool pool(4);
//
//    // �۾��� ������ Ǯ�� �߰�
//    for (int i = 0; i < 8; ++i) {
//        pool.Enqueue(&Example::MemberFunction, &example, i, 3.14);
//        // �����Ϸ����� �Ű������� Ÿ���� ��������� �����մϴ�.
//        // MemberFunction�� �Ű������� int�� double�Դϴ�.
//    }
//
//    return 0;
//}
