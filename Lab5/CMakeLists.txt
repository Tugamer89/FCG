add_executable(butterfly butterfly.cc)
target_link_libraries(butterfly PRIVATE common_deps glm::glm-header-only)

add_dependencies(butterfly CopyResources)

install(TARGETS butterfly DESTINATION bin)
