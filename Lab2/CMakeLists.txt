set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(my_drawing my_drawing.cpp)
target_link_libraries(my_drawing PRIVATE SFML::Graphics)

install(TARGETS my_drawing DESTINATION bin)
