cmake_minimum_required(VERSION 2.8)

project(low_latency_presentation)


if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall -Wextra")
endif()

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library.") # To disable tests of google benchmark
add_subdirectory(google_benchmark benchmark EXCLUDE_FROM_ALL)
add_subdirectory(dictionary_benchmark)

