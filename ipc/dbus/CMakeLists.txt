cmake_minimum_required(VERSION 3.10)
project(dbus)

find_package(Boost COMPONENTS chrono REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUS REQUIRED libsystemd)

add_executable(dbus-server src/dbus_server.cpp)
target_link_libraries(dbus-server ${SDBUS_LIBRARIES})

add_executable(dbus-client src/dbus_client.cpp)
target_link_libraries(dbus-client ${SDBUS_LIBRARIES})

add_executable(dbus-echo src/dbus_echo.cpp)
target_link_libraries(dbus-echo ${SDBUS_LIBRARIES})

add_executable(dbus-test src/dbus_test.cpp)
target_link_libraries(dbus-test ${SDBUS_LIBRARIES})