#include <cassert>
#include <string>
#include <ThreadPool.h>
#include <stdexcept>

void testDestructorWaitsForSubmittedTasks()
{
	constexpr int taskCount = 20;
	std::atomic<int> completedTasks{};

	{
		ThreadPool pool(4);

		for (int task{}; task < taskCount; task++)
		{
			const bool accepted = pool.submit(
				[&completedTasks](int workerId)
			{
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
				[&completedTasks , taskDurationMs](int workerId)
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

void testShutdownCanBeCalledMultipleTimes()
{
	ThreadPool pool(4);

	pool.shutdown();
	pool.shutdown();

	const bool accepted = pool.submit([](int workerId)
	{
		(void) workerId;
	});

	assert(!accepted);
}

void testSubmitFutureReturnsTaskResult()
{
	ThreadPool pool(1);

	auto future = pool.submitFuture([](int workerId)
	{
		(void) workerId;

		return 5;
	});

	assert(future.get() == 5);

	auto stringFuture = pool.submitFuture([](int workerId)
	{
		return std::string{ "worker " } + std::to_string(workerId);
	});

	const std::string result = stringFuture.get();

	assert(result == "worker 0");
}

void testSubmitFuturePropagatesException()
{
	ThreadPool pool(4);

	auto future = pool.submitFuture([](int workerId)
	{
		(void) workerId;

		throw std::runtime_error("boom");
		return 5;
	});

	bool caught = false;

	try
	{
		future.get();
	}
	catch (const std::runtime_error&)
	{
		caught = true;
	}

	assert(caught);
}

int main()
{
	testDestructorWaitsForSubmittedTasks();
	testThreadPoolIsNonCopyableAndNonMovable();
	testSumbitRejectsTasksAfterShutdown();
	testTasksRunInParallel();
	testShutdownCanBeCalledMultipleTimes();
	testSubmitFutureReturnsTaskResult();
	testSubmitFuturePropagatesException();

	return 0;
}
