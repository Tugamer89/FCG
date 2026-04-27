#define GLAD_GL_IMPLEMENTATION
#include <SFML/Window.hpp>
#include <iostream>

#include "glad/gl.h"

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 32;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    settings.majorVersion = 4;
    settings.minorVersion = 5;

    const int window_width = 800;
    const int window_height = 600;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition((desktop.size.x - window_width) / 2,
                                (desktop.size.y - window_height) / 2);

    sf::Window window(sf::VideoMode({window_width, window_height}), "SFML + OpenGL",
                      sf::Style::Default, sf::State::Windowed, settings);
    window.setPosition(centerPosition);
    window.setVerticalSyncEnabled(true);

    if (!window.setActive(true)) {
        std::cerr << "Failure: error during SFML OpenGL Activation." << std::endl;
        return 1;
    }

    sf::ContextSettings gotten = window.getSettings();
    std::cout << "depth bits:" << gotten.depthBits << std::endl;
    std::cout << "stencil bits:" << gotten.stencilBits << std::endl;
    std::cout << "antialiasing level:" << gotten.antiAliasingLevel << std::endl;
    std::cout << "version:" << gotten.majorVersion << "." << gotten.minorVersion << std::endl;

    int version = gladLoadGL(sf::Context::getFunction);
    if (!version) {
        std::cerr << "Failure: error during glad loading." << std::endl;
        return 1;
    }

    std::cout << "GLAD GL version " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    float points[] = {0.f, 0.5f, 0.f, 0.5f, -0.5f, 0.f, -0.5f, -0.5f, 0.f};

    // Vertex Buffer Object
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // Vertex Array Object
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    const char* vertex_shader =
        "#version 410 core\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 410 core\n"
        "out vec4 frag_color;"
        "void main() {"
        "  frag_color = vec4(0.5, 0.0, 0.5, 1.0);"
        "}";

    GLuint vert_s = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_s, 1, &vertex_shader, NULL);
    glCompileShader(vert_s);

    GLuint frag_s = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_s, 1, &fragment_shader, NULL);
    glCompileShader(frag_s);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_s);
    glAttachShader(shader_program, frag_s);
    glLinkProgram(shader_program);

    bool running = true;
    while (running) {
        // handle events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                // end the program
                running = false;
            } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                // adjust the viewport when the window is resized
                glViewport(0, 0, resized->size.x, resized->size.y);
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    // end the program
                    running = false;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.display();
    }

    return 0;
}
