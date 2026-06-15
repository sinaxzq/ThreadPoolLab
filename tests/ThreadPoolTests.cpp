#include <cassert>

#include <ThreadPool.h>

void testDestructorWaitsForSubmittedTasks()
{
	constexpr int taskCount = 20;
	std::atomic<int> completedTasks{};

	{
		ThreadPool pool(4);

		for (int task{}; task < taskCount; task++)
		{
			const bool accepted = pool.submit(
				[&completedTasks](int workerId) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					++completedTasks;
				});

			assert(accepted);
		}
	}

	assert(completedTasks == taskCount);
}

void testThreadPoolIsNonCopyableAndNonMovable()
{
	static_assert(!std::is_copy_constructible_v<ThreadPool>);

	static_assert(!std::is_copy_assignable_v<ThreadPool>);

	static_assert(!std::is_move_constructible_v<ThreadPool>);

	static_assert(!std::is_move_assignable_v<ThreadPool>);
}

void testSumbitRejectsTasksAfterShutdown()
{
	ThreadPool pool(2);

	pool.shutdown();

	const bool accepted = pool.submit([](int)
{
   });

	assert(!accepted);
}

void testTasksRunInParallel()
{
	constexpr int taskCount = 4;
	constexpr int taskDurationMs = 200;
	constexpr int maxExpectedParallelMs = taskDurationMs * (taskCount - 1);

	std::atomic<int> completedTasks{};

	const auto start = std::chrono::steady_clock::now();
	{
		ThreadPool pool(4);

		for (int task{}; task < taskCount; task++)
		{
			const bool accepted = pool.submit(
				[&completedTasks, taskDurationMs](int workerId)
	{
		(void) workerId;

			std::this_thread::sleep_for(std::chrono::milliseconds(taskDurationMs));
			++completedTasks;
				});

			assert(accepted);
		}
	}

	const auto end = std::chrono::steady_clock::now();

	const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	
	assert(completedTasks == taskCount);
assert(elapsedMs < maxExpectedParallelMs);
}

int main()
{
	testDestructorWaitsForSubmittedTasks();
	testThreadPoolIsNonCopyableAndNonMovable();
	testSumbitRejectsTasksAfterShutdown();
	testTasksRunInParallel();
	
	return 0;
}
