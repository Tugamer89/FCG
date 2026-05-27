add_executable(modelling modelling.cc)
target_link_libraries(modelling PRIVATE common_deps glm::glm-header-only)

add_dependencies(modelling CopyResources)

install(TARGETS modelling DESTINATION bin)
