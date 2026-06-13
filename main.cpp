#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

struct WorkResult
{
    int taskId{};
    std::string message;
    long long elapsedMs{};
};

void doWork(int taskId , std::vector<WorkResult>& results)
{
    const auto start = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(1200 - taskId * 100));

    const auto end = std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    WorkResult result{
        taskId,
        "task " + std::to_string(taskId) + " finished",
        elapsedMs
    };

    results[taskId] = result;
}

int main()
{
    const int taskCount = 8;

    std::vector<WorkResult> results(taskCount);

    std::vector<std::thread> threads;

    for (int i = 0; i < taskCount; ++i)
    {
        threads.emplace_back(
            doWork ,
            i ,
            std::ref(results)
        );
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    std::cout << "Results count: " << results.size() << "\n";

    for (const WorkResult& result : results)
    {
        std::cout << result.taskId << " | "
            << result.message << " | "
            << result.elapsedMs << " ms\n";
    }

    return 0;
}