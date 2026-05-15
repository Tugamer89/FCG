set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)

add_executable(butterfly butterfly.cc)
target_link_libraries(butterfly PRIVATE SFML::Window OpenGL::GL glm::glm)

install(TARGETS butterfly DESTINATION bin)

set(LAB5_SHADERS
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab5/vertex.vert"
    "${CMAKE_SOURCE_DIR}/resources/shaders/Lab5/fragment.frag"
)

target_copy_shaders(butterfly Lab5 ${LAB5_SHADERS})
