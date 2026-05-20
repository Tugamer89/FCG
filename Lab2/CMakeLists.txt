add_executable(my_drawing my_drawing.cpp)
target_link_libraries(my_drawing PRIVATE SFML::Graphics project_warnings)

install(TARGETS my_drawing DESTINATION bin)
