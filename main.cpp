#include "ThreadPool.h"

#include <chrono>
#include <iostream>
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
			const bool accepted = pool.submit([taskId , &results](int workerId)
			{
				processTask(taskId , workerId , results);
			});

			if (!accepted)
			{
				std::cout << "Task " << taskId << " was rejected\n";
			}
		}
	}

	const auto totalEnd = std::chrono::steady_clock::now();

	const auto totalElapsedMs =
		std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();

	std::cout << "Results count: " << results.size() << "\n";

	for (const WorkResult& result : results)
	{
		std::cout << "task " << result.taskId
			<< " | worker " << result.workerId
			<< " | " << result.message
			<< " | " << result.elapsedMs << " ms\n";
	}

	std::cout << "Total: " << totalElapsedMs << " ms\n";

	return 0;
}