#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

void incrementManyTimes(int& counter , int times , std::mutex& counterMutex)
{
    int localCounter{};

    for (int i = 0; i < times; ++i)
    {

        ++localCounter;
    }

    std::lock_guard<std::mutex> lock(counterMutex);
    counter += localCounter;
}

int main()
{
    int counter = 0;
    std::mutex counterMutex;

    const int threadCount = 8;
    const int incrementsPerThread = 100000;

    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(
            incrementManyTimes ,
            std::ref(counter) ,
            incrementsPerThread ,
            std::ref(counterMutex)
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