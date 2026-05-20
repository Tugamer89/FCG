add_executable(mesh mesh.cc)
target_link_libraries(mesh PRIVATE common_deps glm::glm-header-only)

add_dependencies(mesh CopyResources)

install(TARGETS mesh DESTINATION bin)
