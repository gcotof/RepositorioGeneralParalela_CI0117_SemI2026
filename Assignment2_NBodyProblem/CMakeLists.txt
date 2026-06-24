cmake_minimum_required(VERSION 3.10)

project(nBody LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(nBody
    src/main.cpp
    src/mpiUtils.cpp
    src/io.cpp
    src/physics.cpp
    src/init.cpp
)

target_include_directories(nBody PRIVATE include)

target_link_libraries(nBody PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)