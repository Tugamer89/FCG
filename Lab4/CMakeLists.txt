add_executable(hello_gl hello_gl.cc)
add_executable(hotshaders hotshaders.cc)

target_link_libraries(hello_gl PRIVATE common_deps)
target_link_libraries(hotshaders PRIVATE common_deps)

add_dependencies(hello_gl CopyResources)
add_dependencies(hotshaders CopyResources)

install(TARGETS hello_gl DESTINATION bin)
install(TARGETS hotshaders DESTINATION bin)
