cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_core MODULE 
    src/cpp_src/pb_methods_and_measures/GreedyAV.cpp
    src/cpp_src/pb_methods_and_measures/GreedyOverCost.cpp
    src/cpp_src/pb_methods_and_measures/Phragmen.cpp
    src/cpp_src/utils/ProjectComparator.cpp
    src/main.cpp 
)

target_include_directories(_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src/cpp_src
)

install(TARGETS _core DESTINATION ${SKBUILD_PROJECT_NAME})