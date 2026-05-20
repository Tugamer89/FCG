add_executable(blockbreaker blockbreaker.cpp)
target_link_libraries(blockbreaker PRIVATE SFML::Graphics project_warnings)
target_include_directories(blockbreaker PRIVATE ${CMAKE_SOURCE_DIR}/resources)

install(TARGETS blockbreaker DESTINATION bin)
