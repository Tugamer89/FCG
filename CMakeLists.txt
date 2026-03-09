cmake_minimum_required(VERSION 4.0)
project(FCG_Labs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Laboratori
add_subdirectory(Lab1)
# add_subdirectory(Lab2)
