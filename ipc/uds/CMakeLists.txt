cmake_minimum_required(VERSION 3.10)
project(uds)

find_package(Boost COMPONENTS chrono REQUIRED)

add_executable(tcp-server src/tcp_server.cpp)
add_executable(tcp-client src/tcp_client.cpp)
add_executable(udp-a src/udp_a.cpp)
add_executable(udp-b src/udp_b.cpp)

add_executable(udp-echo src/udp_echo.cpp)

add_executable(udp-test src/udp_test.cpp)