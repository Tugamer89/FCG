#include <SFML/Window.hpp>
#include <algorithm>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include "include/hotshaders.hh"

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

//////////////////////////
// What and how to draw //
//////////////////////////

class Scene {
   private:
    std::vector<float> points;
    std::vector<unsigned int> indices;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;

   public:
    Scene() { load(); }
    ~Scene() { clean(); }

    void load() {
        // coordinates in clip space! no transformation involved
        points = {
            0.0,  0.5,  0.0,  // red/magenta mix (kind of...), top center
            0.8,  0.0,  0.4,  //

            0.5,  -0.5, 0.3,  // green, bottom right, back
            0.0,  0.8,  0.0,  //

            -0.5, -0.5, 0.3,  // blue, bottom left, back
            0.0,  0.0,  0.8,  //

            0.5,  -0.5, -0.3,  // cyan, bottom right, front
            0.0,  0.8,  0.8,   //

            -0.5, -0.5, -0.3,  // yellow, bottom left, front
            0.8,  0.8,  0.0    //
        };

        // 2 faces, 3 indices per face, CCW order
        indices = {
            0, 3, 4,  //
            0, 2, 1   //
        };

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

    void draw() {
        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw all elements as described by indices
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
};

class Camera {
   public:
    glm::mat4 vp;
    float phi_deg = 0;
    float theta_deg = 0;

    GLint vp_location;

    Camera(GLuint shader_program) {
        vp_location = glGetUniformLocation(shader_program, "vp");

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
        float phi = glm::radians(phi_deg);
        float theta = glm::radians(theta_deg);

        float cp = cos(phi);
        float sp = sin(phi);
        float ct = cos(theta);
        float st = sin(theta);

        // Y-axis rotation matrix (phi)
        glm::mat4 Ry = glm::mat4(cp, 0.0, -sp, 0.0,   //
                                 0.0, 1.0, 0.0, 0.0,  //
                                 sp, 0.0, cp, 0.0,    //
                                 0.0, 0.0, 0.0, 1.0);

        // X-axis rotation matrix (theta)
        glm::mat4 Rx = glm::mat4(1.0, 0.0, 0.0, 0.0,  //
                                 0.0, ct, st, 0.0,    //
                                 0.0, -st, ct, 0.0,   //
                                 0.0, 0.0, 0.0, 1.0);

        // Translation matrix along Z (center z = -2.0)
        glm::mat4 T = glm::mat4(1.0, 0.0, 0.0, 0.0,  //
                                0.0, 1.0, 0.0, 0.0,  //
                                0.0, 0.0, 1.0, 0.0,  //
                                0.0, 0.0, -2.0, 1.0);

        // Projection Matrix
        float fd = 2.0;
        float f_cp = 3.0;
        float ncp = 1.0;

        float A = -(f_cp + ncp) / (f_cp - ncp);
        float B = -(2.0 * f_cp * ncp) / (f_cp - ncp);

        glm::mat4 P = glm::mat4(fd, 0.0, 0.0, 0.0,  //
                                0.0, fd, 0.0, 0.0,  //
                                0.0, 0.0, A, -1.0,  //
                                0.0, 0.0, B, 0.0);

        vp = P * T * Rx * Ry;

        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(vp));
    }
};

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, bool& running) {
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

    Shaders shaders(vertLoc, fragLoc);
    shaders.use();

    Scene scene;
    Camera camera(shaders.program);

    // face culling (temporarily disabled, because the butterfly is not a closed surface)
    // glEnable (GL_CULL_FACE);
    // glCullFace (GL_BACK);

    // depth testing
    glEnable(GL_DEPTH_TEST);

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
                handle(*key_pressed, shaders, running);
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                handle(*mouse, camera);
        }

        scene.draw();
        window.display();
    }

    return 0;
}
