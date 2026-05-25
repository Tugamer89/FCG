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

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

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

# Common Dependencies
add_library(common_deps INTERFACE)
target_link_libraries(common_deps INTERFACE
    project_warnings
    SFML::Window
    OpenGL::GL
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    COMMENT "Copying resources in bin/resources..."
)

# Global Dependecies
find_package(OpenGL REQUIRED)

# Labs
add_subdirectory(Lab1)
add_subdirectory(Lab2)
add_subdirectory(Lab3)
add_subdirectory(Lab4)
add_subdirectory(Lab5)
add_subdirectory(Lab6)

# Installation & Packing
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION bin)
include(CPack)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "fcg-labs")
set(CPACK_PACKAGE_VERSION "2.11.1") # x-release-please-version
