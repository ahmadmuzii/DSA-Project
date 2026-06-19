cmake_minimum_required(VERSION 3.10)
project(CalendarSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common library sources (all implementation files excluding main entry points)
set(LIB_SOURCES
    src/event.cpp
    src/calendar.cpp
    src/file_manager.cpp
    src/ds_utility.cpp
    src/linked_list.cpp
    src/bst.cpp
    src/min_heap.cpp
    src/hash_table.cpp
    src/logger.cpp
)

# Include headers
include_directories(include)

# CLI executable
add_executable(calendar_cli src/main.cpp ${LIB_SOURCES})

# Server executable
add_executable(calendar_server src/server.cpp ${LIB_SOURCES})
target_compile_definitions(calendar_server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# Test executable (doctest provides its own main)
add_executable(test_runner src/test.cpp ${LIB_SOURCES})
