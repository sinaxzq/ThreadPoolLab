#include "ThreadPool.h"

#include <utility>

ThreadPool::ThreadPool(int workerCount)
{
    for (int workerId = 0; workerId < workerCount; ++workerId)
    {
        workers.emplace_back(&ThreadPool::workerLoop , this , workerId);
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

bool ThreadPool::submit(std::function<void(int)> task)
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);

        if (stopRequested)
        {
            return false;
        }

        taskQueue.push(std::move(task));
    }

    taskCv.notify_one();
    return true;
}

void ThreadPool::workerLoop(int workerId)
{
    while (true)
    {
        std::function<void(int)> task;

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

        task(workerId);
    }
}

void ThreadPool::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);

        if (stopRequested)
        {
            return;
        }

        stopRequested = true;
    }

    taskCv.notify_all();

    for (auto& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}