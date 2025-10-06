cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

option(BUILD_STATIC "Build static OR-Tools for packaging" OFF)

if(BUILD_STATIC)
    include(FetchContent)

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(BUILD_DEPS ON CACHE BOOL "Build dependencies automatically" FORCE)
    set(ORTOOLS_BUILD_DEPS ON CACHE BOOL "Build dependencies" FORCE)
    set(ORTOOLS_USE_HIGHS OFF CACHE BOOL "Disable HiGHS" FORCE)
    set(ORTOOLS_USE_SCIP OFF CACHE BOOL "Disable SCIP" FORCE)
    set(ORTOOLS_USE_CPLEX OFF CACHE BOOL "Disable CPLEX" FORCE)
    set(ORTOOLS_USE_XPRESS OFF CACHE BOOL "Disable Xpress" FORCE)
    set(ORTOOLS_USE_PDLP OFF CACHE BOOL "Disable PDLP" FORCE)

    FetchContent_Declare(
        ortools
        GIT_REPOSITORY https://github.com/google/or-tools.git
        GIT_TAG v9.11
    )
    FetchContent_MakeAvailable(ortools)
else()
    find_package(ortools REQUIRED)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
    message(STATUS "Using prebuilt OR-Tools")
endif()

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Enabling coverage flags")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage only supported with GCC/Clang")
    endif()
endif()

pybind11_add_module(_core MODULE
    src/cpp_src/pb_rules_and_measures/Greedy.cpp
    src/cpp_src/pb_rules_and_measures/GreedyOverCost.cpp
    src/cpp_src/pb_rules_and_measures/MesApr.cpp
    src/cpp_src/pb_rules_and_measures/MesCost.cpp
    src/cpp_src/pb_rules_and_measures/Phragmen.cpp
    src/cpp_src/utils/Math.cpp
    src/cpp_src/utils/ProjectComparator.cpp
    src/main.cpp
)

target_include_directories(_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src/cpp_src
)

target_link_libraries(_core PRIVATE ortools::ortools)

install(TARGETS _core DESTINATION ${SKBUILD_PROJECT_NAME})
