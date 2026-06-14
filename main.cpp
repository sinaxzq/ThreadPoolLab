#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

struct WorkResult
{
    int taskId{};
    std::string message;
    long long elapsedMs{};
};

class ThreadPool
{
public:
    explicit ThreadPool(int workerCount)
    {
        for (int i = 0; i < workerCount; ++i)
        {
            workers.emplace_back(&ThreadPool::workerLoop , this);
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(taskMutex);
            stopRequested = true;
        }

        taskCv.notify_all();

        for (std::thread& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void submit(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(taskMutex);
            taskQueue.push(std::move(task));
        }

        taskCv.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> taskQueue;
    std::mutex taskMutex;
    std::condition_variable taskCv;
    bool stopRequested = false;

    void workerLoop()
    {
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(taskMutex);

                taskCv.wait(lock , [this]()
 {
     return !taskQueue.empty() || stopRequested;
                });

                if (taskQueue.empty() && stopRequested)
                {
                    break;
                }

                task = std::move(taskQueue.front());
                taskQueue.pop();
            }

            task();
        }
    }
};

void processTask(int taskId , std::vector<WorkResult>& results)
{
    const auto start = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(300 + taskId * 25));

    const auto end = std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    results[taskId] = WorkResult{
        taskId,
        "task " + std::to_string(taskId) + " done",
        elapsedMs
    };
}

int main()
{
    const int taskCount = 20;
    const int workerCount = 4;

    std::vector<WorkResult> results(taskCount);

    const auto totalStart = std::chrono::steady_clock::now();

    {
        ThreadPool pool(workerCount);

        for (int taskId = 0; taskId < taskCount; ++taskId)
        {
            pool.submit([taskId , &results]()
 {
     processTask(taskId , results);
            });
        }
    }

    const auto totalEnd = std::chrono::steady_clock::now();

    const auto totalElapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();

    std::cout << "Results count: " << results.size() << "\n";

    for (const WorkResult& result : results)
    {
        std::cout << "task " << result.taskId
            << " | " << result.message
            << " | " << result.elapsedMs << " ms\n";
    }

    std::cout << "Total: " << totalElapsedMs << " ms\n";

    return 0;
}