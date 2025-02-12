cmake_minimum_required(VERSION 3.14)

project(flukyTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Tell cmake to include everything in compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(fluky REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

find_package(nanobench REQUIRED)

# ---- Tests ----

add_executable(fluky_test
    source/fluky_test.cpp
    source/rng_performance.cpp
)
target_link_libraries(
    fluky_test PRIVATE
    fluky::fluky
    Catch2::Catch2WithMain
    nanobench::nanobench
)
target_compile_features(fluky_test PRIVATE cxx_std_20)

catch_discover_tests(fluky_test)

# ---- End-of-file commands ----

add_folders(Test)

# ---- RNG executables for testing with PractRand
add_executable(jsf64 source/jsf64.cpp)
target_link_libraries(
    jsf64 PRIVATE
    fluky::fluky
)

add_executable(sfc64 source/sfc64.cpp)
target_link_libraries(
    sfc64 PRIVATE
    fluky::fluky
)

add_executable(romu_duo source/romu_duo.cpp)
target_link_libraries(
    romu_duo PRIVATE
    fluky::fluky
)

add_executable(romu_trio source/romu_trio.cpp)
target_link_libraries(
    romu_trio PRIVATE
    fluky::fluky
)

add_executable(xoshiro256ss source/xoshiro256ss.cpp)
target_link_libraries(
    xoshiro256ss PRIVATE
    fluky::fluky
)

add_executable(pcg64_dxsm source/pcg64_dxsm.cpp)
target_link_libraries(
    pcg64_dxsm PRIVATE
    fluky::fluky
)
