cmake_minimum_required(VERSION 3.20)

project(ThreadPoolLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(ThreadPoolCore
src/ThreadPool.cpp
)

target_include_directories(ThreadPoolCore PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ThreadPoolCore PUBLIC
Threads::Threads
)

add_executable(ThreadPoolLab
main.cpp
)

target_link_libraries(ThreadPoolLab PRIVATE
ThreadPoolCore
)

add_executable(ThreadPoolTests
tests/ThreadPoolTests.cpp
)

target_link_libraries(ThreadPoolTests PRIVATE
ThreadPoolCore
)

enable_testing()

add_test(NAME ThreadPoolTests COMMAND ThreadPoolTests)
