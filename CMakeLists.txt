cmake_minimum_required(VERSION 3.20)

project(ThreadPoolLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ThreadPoolLab
    main.cpp
    ThreadPool.cpp
)

add_executable(ThreadPoolTests
    tests/ThreadPoolTests.cpp
    ThreadPool.cpp
)

target_include_directories(ThreadPoolTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

enable_testing()

add_test(NAME ThreadPoolTests COMMAND ThreadPoolTests)