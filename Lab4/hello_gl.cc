#define GLAD_GL_IMPLEMENTATION
#include <SFML/Window.hpp>
#include <iostream>

#include "glad/gl.h"

int main() {
    // Options for OpenGL context, to be kept in sync with GLAD options!
    sf::ContextSettings settings;
    settings.depthBits = 32;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    settings.majorVersion = 4;
    settings.minorVersion = 1;

    const int window_width = 800;
    const int window_height = 600;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition((desktop.size.x - window_width) / 2,
                                (desktop.size.y - window_height) / 2);

    // Create the window with chosen options
    sf::Window window(sf::VideoMode({window_width, window_height}), "SFML + OpenGL",
                      sf::Style::Default, sf::State::Windowed, settings);
    window.setPosition(centerPosition);
    window.setVerticalSyncEnabled(true);

    // Activate the window's OpenGL context
    if (!window.setActive(true)) {
        std::cerr << "Failure: error during SFML OpenGL Activation." << std::endl;
        return 1;
    }

    // Check what we have received back
    sf::ContextSettings gotten = window.getSettings();
    std::cout << "depth bits:" << gotten.depthBits << std::endl;
    std::cout << "stencil bits:" << gotten.stencilBits << std::endl;
    std::cout << "antialiasing level:" << gotten.antiAliasingLevel << std::endl;
    std::cout << "version:" << gotten.majorVersion << "." << gotten.minorVersion << std::endl;

    // GLAD magic!
    // SFML provides a function to retrieve OpenGL's functions at runtime
    // GLAD uses it to load all the OpenGL functions that are needed for our configuration
    int version = gladLoadGL(sf::Context::getFunction);
    if (!version) {
        std::cerr << "Failure: error during glad loading." << std::endl;
        return 1;
    }
    // Final check that we got what we want
    std::cout << "GLAD GL version" << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    // run the main loop
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

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw here

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    return 0;
}
