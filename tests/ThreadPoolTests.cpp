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


int main()
{
	testDestructorWaitsForSubmittedTasks();
	testThreadPoolIsNonCopyableAndNonMovable();

	return 0;
}
