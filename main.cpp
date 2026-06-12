#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void doWork(const std::string& name)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << name << " finished\n";
}

int main()
{
    const auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    threads.emplace_back(doWork , "task A");
    threads.emplace_back(doWork , "task B");
    threads.emplace_back(doWork , "task C");

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    const auto end = std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total: " << elapsedMs << " ms\n";

    return 0;
}