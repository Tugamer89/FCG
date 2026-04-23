cmake_minimum_required(VERSION 3.28)
project(FCG_Labs LANGUAGES CXX)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall 
    -funsigned-char
    -Wno-error=narrowing
  >
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Laboratori
add_subdirectory(Lab1)
add_subdirectory(Lab2)
add_subdirectory(Lab3)

# Installation & Packing
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION bin)
include(CPack)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "fcg-labs")
set(CPACK_PACKAGE_VERSION "2.0.0") # x-release-please-version
