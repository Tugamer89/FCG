#include "hotshaders.hh"

#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>

const char* vertLoc = "Lab4/vertex.vert";
const char* fragLoc = "Lab4/fragment.frag";

struct Setup {
    sf::Window* window;

    Setup() {
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

        window = new sf::Window(sf::VideoMode({window_width, window_height}), "SFML + OpenGL",
                                sf::Style::Default, sf::State::Windowed, settings);
        window->setPosition(centerPosition);
        window->setVerticalSyncEnabled(true);

        if (!window->setActive(true)) {
            std::cerr << "Failure: error during SFML OpenGL Activation." << std::endl;
            exit(1);
        }
        sf::ContextSettings gotten = window->getSettings();

        std::cout << "depth bits: " << gotten.depthBits << std::endl;
        std::cout << "stencil bits: " << gotten.stencilBits << std::endl;
        std::cout << "antialiasing level: " << gotten.antiAliasingLevel << std::endl;
        std::cout << "SFML GL version: " << gotten.majorVersion << "." << gotten.minorVersion
                  << std::endl;

        int version = gladLoadGL(sf::Context::getFunction);
        if (!version) {
            std::cerr << "Failure: error during glad loading." << std::endl;
            exit(1);
        }
        std::cout << "GLAD GL version: " << GLAD_VERSION_MAJOR(version) << "."
                  << GLAD_VERSION_MINOR(version) << std::endl;
    }

    ~Setup() { delete window; }
};

struct Scene {
    std::vector<float> points;
    GLuint vbo;
    GLuint vao;

    Scene() { load(); }
    ~Scene() { clean(); }

    void load() {
        // coordinates in clip space! no transformation involved
        points = {
            0.0f,  0.5f,  0.0f,  // x,y,z of first point.
            0.5f,  -0.5f, 0.0f,  // x,y,z of second point.
            -0.5f, -0.5f, 0.0f   // x,y,z of third point.
        };

        vbo = 0;
        // we want just one buffer, and we retrieve the name OpenGL assigns to it.
        glGenBuffers(1, &vbo);
        // bind it as the current ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // transfer data from CPU RAM to GPU RAM.
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        vao = 0;
        // we want just one buffer container, and we retrieve the name OpenGL assigns to it.
        glGenVertexArrays(1, &vao);
        // bind it as the current vao.
        glBindVertexArray(vao);
        // we describe how the attribute 0 is organized inside our buffer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        // we enable attribute 0 to be passed as input to the vertex shader
        glEnableVertexAttribArray(0);
    }

    void clean() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    // // when data will be dynamically loaded, reloading will be useful
    // void reload() {
    //     clean();
    //     load();
    // }
};

/////////////
// Draw!!! //
/////////////

float accumulator = 0.0;
float next_red = 0.8;

void draw(Scene& scene, Shaders& shaders, float elapsed) {
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // get the location of the uniform variable
    GLint loc = glGetUniformLocation(shaders.program, "foreground");
    
    accumulator += elapsed;
    if (accumulator > 2.0) {
        accumulator = 0.0;
    }

    if (accumulator > 1.0) {
        glUniform3f(loc, next_red, 0.4, 0.0);
    } else {
        glUniform3f(loc, 0.0, 0.4, 0.8);
    }

    // Draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Scene& scene, Shaders& shaders, bool& running) {
    switch (key.scancode) {
        case sf::Keyboard::Scancode::Space:
            shaders.reload(vertLoc, fragLoc);
            glUseProgram(shaders.program);
            return;
        case sf::Keyboard::Scancode::Escape:
            running = false;
            return;
        default:
            return;
    }
}

int main() {
    // setup SFML + OpenGL, using GLAD
    Setup setup;
    sf::Window& window = *setup.window;

    // create a default scene
    Scene scene;

    // load shaders from files
    Shaders shaders(vertLoc, fragLoc);

    // Put the shader program, and the VAO, in focus in OpenGL's state machine
    // these can changed dynamically as needed in the loop too.
    glUseProgram(shaders.program);
    glBindVertexArray(scene.vao);

    ///////////////
    // Main loop //
    ///////////////

    sf::Clock clock;

    bool running = true;
    while (running) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle(*key_pressed, scene, shaders, running);
        }

        draw(scene, shaders, clock.restart().asSeconds());

        window.display();
    }

    return 0;
}
