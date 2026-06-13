#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

void incrementManyTimes(std::atomic<int>& counter , int times)
{
    for (int i = 0; i < times; ++i)
    {
        ++counter;
    }
}

int main()
{
    std::atomic<int> counter{ 0 };

    const int threadCount = 8;
    const int incrementsPerThread = 100000;

    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(
            incrementManyTimes ,
            std::ref(counter) ,
            incrementsPerThread
        );
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    std::cout << "Expected: " << threadCount * incrementsPerThread << "\n";
    std::cout << "Actual:   " << counter << "\n";

    return 0;
}