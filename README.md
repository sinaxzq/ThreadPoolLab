# ThreadPoolLab

[![CMake CI](https://github.com/sinaxzq/ThreadPoolLab/actions/workflows/ci.yml/badge.svg)](https://github.com/sinaxzq/ThreadPoolLab/actions/workflows/ci.yml)

A small C++ thread pool project focused on task submission, futures, shutdown behavior, and automated tests.

## What it demonstrates

* `std::thread` worker ownership
* `join()` and thread lifecycle
* `std::mutex`, `std::lock_guard`, and `std::unique_lock`
* `std::condition_variable` with `wait`, `notify_one`, and `notify_all`
* task queue synchronization
* explicit `shutdown()`
* RAII-based cleanup in the destructor
* non-copyable and non-movable resource-owning class design
* `std::function` task storage
* `std::future` and `std::packaged_task` for returning task results
* basic assert-based tests with CTest

## Features

* Fixed number of worker threads
* Submit fire-and-forget tasks with `submit`
* Submit tasks with return values using `submitFuture`
* Rejects new tasks after shutdown
* Destructor waits for submitted tasks to finish
* Supports `future<int>`, `future<std::string>`, and `future<void>` style tasks
* Propagates task exceptions through `future.get()`

## Build

PowerShell:

```
cmake -S . -B build
cmake --build build --config Debug
```

## Run demo

```
.\build\Debug\ThreadPoolLab.exe
```

## Run tests

```
ctest --test-dir build -C Debug --output-on-failure
```

## Status

This is an educational concurrency project, not a production-grade thread pool.

Intentionally not implemented:

* task cancellation
* priority queues
* work stealing
* pause/resume
* dynamic worker resizing
* production benchmarking
* C++20 `std::jthread`
* move-only task wrapper
