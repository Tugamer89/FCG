#define GLAD_GL_IMPLEMENTATION
#include <SFML/Window.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "glad/gl.h"

////////////////////
// How it's drawn //
////////////////////

#include "./include/hotshaders.hh"

const char* vertLoc = "resources/shaders/Lab5/vertex.vert";
const char* fragLoc = "resources/shaders/Lab5/fragment.frag";

/////////////////////////////
// Window and OpenGL setup //
/////////////////////////////

class Setup {
   public:
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

//////////////////
// What to draw //
//////////////////

class Scene {
   public:
    std::vector<float> points;
    std::vector<unsigned int> indices;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    bool mouse_left_pressed = false;
    bool mouse_start_drag = false;

    float phi_deg = 0;
    float theta_deg = 0;

    Scene() { load(); }
    ~Scene() { clean(); }

    void load() {
        // coordinates in clip space! no transformation involved
        points = {0.0,  0.5,  0.0,  // red/magenta mix (kind of...), top center
                  0.8,  0.0,  0.4,

                  0.5,  -0.5, 0.3,  // green, bottom right, back
                  0.0,  0.8,  0.0,

                  -0.5, -0.5, 0.3,  // blue, bottom left, back
                  0.0,  0.0,  0.8,

                  0.5,  -0.5, -0.3,  // cyan, bottom right, front
                  0.0,  0.8,  0.8,

                  -0.5, -0.5, -0.3,  // yellow, bottom left, front
                  0.8,  0.8,  0.0};

        // 2 faces, 3 indices per face, CCW order
        indices = {0, 3, 4, 0, 2, 1};

        // we want just one buffer, and we retrieve the name OpenGL assigns to it.
        glGenBuffers(1, &vbo);
        // bind it as the current VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // transfer data from CPU RAM to GPU RAM.
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        // we want just one buffer container, and we retrieve the name OpenGL assigns to it.
        glGenVertexArrays(1, &vao);
        // bind it as the current vao.
        glBindVertexArray(vao);

        // Attribute 0: position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Attribute 1: color (r, g, b)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // element buffer object
        GLuint ebo;
        glGenBuffers(1, &ebo);
        // MUST be bound after the VAO's binding!
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    }

    void clean() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};

class Camera {
   public:
    float phi_deg = 0;
    float theta_deg = 0;

    GLint phi_deg_location;
    GLint theta_deg_location;

    Camera(GLuint shader_program) {
        phi_deg_location = glGetUniformLocation(shader_program, "phi_deg");
        theta_deg_location = glGetUniformLocation(shader_program, "theta_deg");

        update();
    }

    void drag(sf::Vector2f dpos) {
        const float scale = 0.1;

        phi_deg += dpos.x * scale;
        theta_deg += dpos.y * scale;

        theta_deg = std::clamp(theta_deg, -90.f, 90.f);

        update();
    }

   private:
    void update() {
        glUniform1f(phi_deg_location, phi_deg);
        glUniform1f(theta_deg_location, theta_deg);
    }
};

/////////////
// Draw!!! //
/////////////

void draw(Scene& scene) {
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all elements as described by indices
    glDrawElements(GL_TRIANGLES, scene.indices.size(), GL_UNSIGNED_INT, 0);
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

void handle(const sf::Event::MouseMoved& mouse, Camera& camera) {
    static sf::Vector2f prev_pos = {0, 0};

    sf::Vector2f mouse_pos(mouse.position);
    sf::Vector2f dpos = prev_pos - mouse_pos;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        camera.drag(dpos);
    }

    prev_pos = mouse_pos;
}

//////////
// Main //
//////////

int main() {
    // setup SFML + OpenGL, using GLAD
    Setup setup;
    sf::Window& window = *setup.window;

    Scene scene;
    Shaders shaders(vertLoc, fragLoc);
    Camera camera(shaders.program);

    // face culling (temporarily disabled, because the butterfly is not a closed surface)
    // glEnable (GL_CULL_FACE);
    // glCullFace (GL_BACK);

    // depth testing
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaders.program);
    glBindVertexArray(scene.vao);

    ///////////////
    // Main Loop //
    ///////////////

    bool running = true;
    while (running) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle(*key_pressed, scene, shaders, running);
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                handle(*mouse, camera);
        }

        draw(scene);
        window.display();
    }

    return 0;
}
