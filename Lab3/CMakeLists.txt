set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# blockbreaker
add_executable(blockbreaker blockbreaker.cpp)
target_link_libraries(blockbreaker PRIVATE SFML::Graphics project_warnings)

install(TARGETS blockbreaker DESTINATION bin)
