set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)

add_executable(mesh mesh.cc)
target_link_libraries(mesh PRIVATE SFML::Window OpenGL::GL glm::glm-header-only)

install(TARGETS mesh DESTINATION bin)

set(LAB6_SHADERS
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/gouraud_vertex.vert"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/gouraud_fragment.frag"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/phong_vertex.vert"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/phong_fragment.frag"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/flat_vertex.vert"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab6/flat_fragment.frag"
    "${CMAKE_SOURCE_DIR}/resources/meshes/bunny.off"
)

target_copy_shaders(mesh Lab6 ${LAB6_SHADERS})
