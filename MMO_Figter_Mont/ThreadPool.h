#include <iostream>
#include <Windows.h>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>

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

    // 멤버 함수를 스레드 풀에 추가하는 함수
    template<typename T, typename... Args>
    void Enqueue(void (T::* memberFunction)(Args...), T* obj, Args... args) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // 작업을 큐에 추가합니다.
            tasks.emplace([=]() { std::invoke(memberFunction, obj, args...); });
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

// 예제 클래스 정의
class Example {
public:
    // 멤버 함수
    void MemberFunction(int i, double d) {
        std::cout << "Member function called with: " << i << " and " << d << std::endl;
    }
};

//int main() {
//    // 예제 객체 생성
//    Example example;
//    // 스레드 풀 생성
//    ThreadPool pool(4);
//
//    // 작업을 스레드 풀에 추가
//    for (int i = 0; i < 8; ++i) {
//        pool.Enqueue(&Example::MemberFunction, &example, i, 3.14);
//        // 컴파일러에게 매개변수의 타입을 명시적으로 전달합니다.
//        // MemberFunction의 매개변수는 int와 double입니다.
//    }
//
//    return 0;
//}
