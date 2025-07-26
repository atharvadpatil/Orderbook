cmake_minimum_required(VERSION 3.10)
project(OrderbookTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(OrderbookTests
    Tests/tests.cpp
    Orderbook.cpp
)

target_link_libraries(OrderbookTests
    gtest
    gtest_main
    pthread
)

add_test(NAME OrderbookTest COMMAND OrderbookTests)