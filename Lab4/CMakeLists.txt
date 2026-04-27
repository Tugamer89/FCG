set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)

add_executable(hello_gl hello_gl.cc)
target_link_libraries(hello_gl PRIVATE SFML::Window OpenGL::GL)

install(TARGETS hello_gl DESTINATION bin)
