set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)

add_executable(hello_gl hello_gl.cc)
target_link_libraries(hello_gl PRIVATE SFML::Window OpenGL::GL)

install(TARGETS hello_gl DESTINATION bin)

add_executable(hotshaders hotshaders.cc)
target_link_libraries(hotshaders PRIVATE SFML::Window OpenGL::GL)

install(TARGETS hotshaders DESTINATION bin)

set(LAB4_SHADERS
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab4/vertex.vert"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab4/fragment.frag"
)

target_copy_shaders(hotshaders Lab4 ${LAB4_SHADERS})
