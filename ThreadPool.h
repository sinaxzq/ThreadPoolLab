#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
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

    bool submit(std::function<void(int)> task);
    
    void shutdown();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void(int)>> taskQueue;
    std::mutex taskMutex;
    std::condition_variable taskCv;
    bool stopRequested = false;

    void workerLoop(int workerId);
};