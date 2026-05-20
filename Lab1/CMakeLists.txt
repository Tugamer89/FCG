add_executable(main_text_echo src/main_text_echo.cpp)
add_executable(main_text_editor src/main_text_editor.cpp)

target_link_libraries(main_text_echo PRIVATE SFML::Graphics project_warnings)
target_link_libraries(main_text_editor PRIVATE SFML::Graphics project_warnings)

install(TARGETS main_text_echo DESTINATION bin)
install(TARGETS main_text_editor DESTINATION bin)
