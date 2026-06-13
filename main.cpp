#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

std::mutex coutMutex;

void safePrint(const std::string& message)
{
    std::lock_guard<std::mutex> lock(coutMutex);

    std::cout << message << "\n";
}

void doWork(const std::string& name)
{
    safePrint(name + " started");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    safePrint(name + " finished");
}

int main()
{
    const auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    safePrint("Before A");
    threads.emplace_back(doWork , "task A");

    safePrint("Before B");
    threads.emplace_back(doWork , "task B");

    safePrint("Before C");
    threads.emplace_back(doWork , "task C");

    safePrint("Before join loop");

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    const auto end = std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    safePrint("Total: " + std::to_string(elapsedMs) + " ms");

    return 0;
}