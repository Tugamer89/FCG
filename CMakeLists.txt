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

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# --- HELPER FUNCTION ---
# Function for copying resource files (e.g., shaders) to the build directory of the target.
# Use: target_copy_shaders(<TARGET_NAME> <FOLDER_NAME_LAB> <SHADER_1> <SHADER_2> ...)
function(target_copy_shaders TARGET_NAME FOLDER_NAME)
    foreach(SHADER_FILE IN LISTS ARGN)
        get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME)
        
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources/shaders/${FOLDER_NAME}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE}"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/resources/shaders/${FOLDER_NAME}/${SHADER_NAME}"
            COMMENT "Copying the ${SHADER_NAME} shader for ${TARGET_NAME}"
        )
    endforeach()
endfunction()

# Labs
add_subdirectory(Lab1)
add_subdirectory(Lab2)
add_subdirectory(Lab3)
add_subdirectory(Lab4)
add_subdirectory(Lab5)

# Installation & Packing
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION bin)
include(CPack)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "fcg-labs")
set(CPACK_PACKAGE_VERSION "2.7.0") # x-release-please-version
