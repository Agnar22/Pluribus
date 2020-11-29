cmake_minimum_required(VERSION 3.10)
project(Pluribus)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -pthread -lpthread -Ofast -march=native")


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)
if (Boost_FOUND)
    message("Boost found.")
    include_directories(${BOOST_INCLUDE_DIRS})
elseif (NOT Boost_FOUND)
    error("Boost not found.")
endif()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(libs/googletest)
