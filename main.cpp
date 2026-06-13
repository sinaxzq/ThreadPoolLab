#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct WorkResult
{
    int taskId{};
    int workerId{};
    std::string message;
    long long elapsedMs{};
};

void processTask(int taskId , int workerId , std::vector<WorkResult>& results)
{
    const auto start = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(300 + taskId * 25));

    const auto end = std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    results[taskId] = WorkResult{
        taskId,
        workerId,
        "task " + std::to_string(taskId) + " done",
        elapsedMs
    };
}

void workerLoop(
    int workerId ,
    int taskCount ,
    int& nextTaskId ,
    std::mutex& taskMutex ,
    std::vector<WorkResult>& results)
{
    while (true)
    {
        int taskId = -1;

        {
            std::lock_guard<std::mutex> lock(taskMutex);

            if (nextTaskId >= taskCount)
            {
                break;
            }

            taskId = nextTaskId;
            ++nextTaskId;
        }

        processTask(taskId , workerId , results);
    }
}

int main()
{
    const int taskCount = 20;
    const int workerCount = 4;

    int nextTaskId = 0;
    std::mutex taskMutex;

    std::vector<WorkResult> results(taskCount);
    std::vector<std::thread> workers;

    const auto totalStart = std::chrono::steady_clock::now();

    for (int workerId = 0; workerId < workerCount; ++workerId)
    {
        workers.emplace_back(
            workerLoop ,
            workerId ,
            taskCount ,
            std::ref(nextTaskId) ,
            std::ref(taskMutex) ,
            std::ref(results)
        );
    }

    for (std::thread& worker : workers)
    {
        worker.join();
    }

    const auto totalEnd = std::chrono::steady_clock::now();

    const auto totalElapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();

    std::cout << "Results count: " << results.size() << "\n";

    for (const WorkResult& result : results)
    {
        std::cout << "task " << result.taskId
            << " | worker " << result.workerId
            << " | " << result.elapsedMs << " ms\n";
    }

    std::cout << "Total: " << totalElapsedMs << " ms\n";

    return 0;
}