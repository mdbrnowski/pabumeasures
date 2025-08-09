cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_core MODULE 
    src/main.cpp 
    src/cpp_src/ProjectComparator.cpp
)

install(TARGETS _core DESTINATION ${SKBUILD_PROJECT_NAME})
