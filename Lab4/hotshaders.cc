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
    std::vector<unsigned int> indices;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    GLint mod_color_location;
    static constexpr float darken[3] = {0.2, 0.2, 0.2};
    static constexpr float lighten[3] = {-0.2, -0.2, -0.2};

    Scene() { load(); }
    ~Scene() { clean(); }

    void load() {
        // coordinates in clip space! no transformation involved
        points = {
            -0.7f, 0.5f,  0.0f,  // x,y,x
            0.2f,  0.2f,  0.2f,  // r,g,b

            0.0f,  0.5f,  0.0f,  // x,y,z
            0.8f,  0.0f,  0.0f,  // r,g,b

            0.5f,  -0.5f, 0.0f,  // x,y,z
            0.0f,  0.8f,  0.0f,  // r,g,b

            -0.5f, -0.5f, 0.0f,  // x,y,z
            0.0f,  0.0f,  0.8f,  // r,g,b

            0.7f,  0.5f,  0.0f,  // x,y,z
            0.8f,  0.8f,  0.8f,  // r,g,b

            0.7f,  -0.1f, 0.5f,  // x,y,z
            0.5f,  0.5f,  0.5f,  // r,g,b

            -0.7f, -0.1f, 0.5f,  // x,y,z
            0.5f,  0.5f,  0.5f,  // r,g,b

            0.0f,  0.2f,  0.5f,  // x,y,z
            0.5f,  0.5f,  0.5f,  // r,g,b
        };

        indices = {
            3, 1, 0,  // 1st face
            3, 2, 1,  // 2nd face
            1, 2, 4,  // 3rd face
            7, 6, 5,  // 4th face
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    }

    void clean() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};

/////////////
// Draw!!! //
/////////////

float accumulator = 0.0;

void draw(Scene& scene, Shaders& shaders, float elapsed) {
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    accumulator += elapsed;

    if (accumulator < 1.f) {
        glUniform3fv(scene.mod_color_location, 1, scene.lighten);
    } else if (accumulator < 2.f || (accumulator >= 3.f && accumulator < 4.f)) {
        glUniform3f(scene.mod_color_location, 0.f, 0.f, 0.f);
    } else if (accumulator < 3.f) {
        glUniform3fv(scene.mod_color_location, 1, scene.darken);
    } else {
        accumulator = 0.f;
    }

    glDrawElements(GL_TRIANGLES, scene.indices.size(), GL_UNSIGNED_INT, (void*)0);
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

    Scene scene;
    Shaders shaders(vertLoc, fragLoc);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glUseProgram(shaders.program);
    glBindVertexArray(scene.vao);

    scene.mod_color_location = glGetUniformLocation(shaders.program, "mod_color");

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
