#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool
{
public:
    explicit ThreadPool(int workerCount);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ~ThreadPool();

    void shutdown();

    bool submit(std::function<void(int)> task);

    template <typename Func>
    auto submitFuture(Func task) -> std::future<std::invoke_result_t<Func, int>>
    {
        using Result = std::invoke_result_t<Func, int>;

        auto packagedTask = std::make_shared<std::packaged_task<Result(int)>>(std::move(task));

        std::future<Result> future = packagedTask->get_future();

        {
            std::lock_guard<std::mutex> lock(taskMutex);

            if (stopRequested)
            {
                throw std::runtime_error("Cannot submit task after shutdown");
            }

            taskQueue.push([packagedTask](int workerId) { (*packagedTask)(workerId); });
        }

        taskCv.notify_one();

        return future;
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void(int)>> taskQueue;
    std::mutex taskMutex;
    std::condition_variable taskCv;
    bool stopRequested = false;

    void workerLoop(int workerId);
};