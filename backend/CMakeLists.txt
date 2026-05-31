cmake_minimum_required(VERSION 3.10)
project(CalendarSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/event.cpp
    src/calendar.cpp
    src/file_manager.cpp
    src/ds_utility.cpp
    src/linked_list.cpp
    src/bst.cpp
    src/min_heap.cpp
    src/hash_table.cpp
)

set(SERVER_SOURCES
    src/server.cpp
    src/event.cpp
    src/calendar.cpp
    src/file_manager.cpp
    src/ds_utility.cpp
    src/linked_list.cpp
    src/bst.cpp
    src/min_heap.cpp
    src/hash_table.cpp
)

# Include headers
include_directories(include)

# CLI executable
add_executable(calendar_cli ${SOURCES})

# Server executable (only if httplib.h is present)
add_executable(calendar_server ${SERVER_SOURCES})
target_compile_definitions(calendar_server PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# Test executable
add_executable(test_runner src/test.cpp ${SERVER_SOURCES})
target_link_libraries(test_runner)
