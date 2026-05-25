#define GLAD_GL_IMPLEMENTATION
#include "mesh.hh"

#include <SFML/Window.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>

#include "glad/gl.h"
#include "include/hotshaders.hh"

// Gouraud shaders
const char* const gouraud_vertex_shader_path = "resources/shaders/Lab6/gouraud_vertex.vert";
const char* const gouraud_fragment_shader_path = "resources/shaders/Lab6/gouraud_fragment.frag";

// Phong shaders
const char* const phong_vertex_shader_path = "resources/shaders/Lab6/phong_vertex.vert";
const char* const phong_fragment_shader_path = "resources/shaders/Lab6/phong_fragment.frag";

// Flat shaders
const char* const flat_vertex_shader_path = "resources/shaders/Lab6/flat_vertex.vert";
const char* const flat_fragment_shader_path = "resources/shaders/Lab6/flat_fragment.frag";

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
        const int window_height = 800;

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

////////////////////
// Camera + World //
////////////////////

class CameraLights {
   private:
    // Uniform locations
    GLint tm_loc;
    GLint camera_pos_loc;
    GLint direct_pos_loc;
    GLint direct_val_loc;
    GLint ambient_val_loc;
    GLint ambient_loc;
    GLint diffuse_loc;
    GLint specular_loc;
    GLint shininess_loc;

    glm::vec3 direct_val = {1.f, 1.f, 1.f};      // rgb
    glm::vec3 ambient_val = {0.2f, 0.2f, 0.2f};  // rgb
    glm::vec3 ambient = {0.1f, 0.7f, 0.8f};      // xyz
    glm::vec3 diffuse = {0.1f, 0.7f, 0.8f};      // xyz
    glm::vec3 specular = {0.5f, 0.5f, 0.5f};     // xyz
    float shininess = 64.f;                      // scalar

    float phi_deg = 210.0;
    float theta_deg = 2.0;

    const float normal_fd = 4.0;
    const float tele_fd = 100.0;
    const float wide_fd = 1.5;

    float fd;  // focal distance
    float od;  // object distance

   public:
    explicit CameraLights(const Shaders& shaders) {
        update_locations(shaders.program);
        view_normal();
    }

    // Refresh uniform locations
    void update_locations(GLuint program) {
        tm_loc = glGetUniformLocation(program, "tm");
        camera_pos_loc = glGetUniformLocation(program, "camera_pos");
        direct_pos_loc = glGetUniformLocation(program, "light.direct_pos");
        direct_val_loc = glGetUniformLocation(program, "light.direct_val");
        ambient_val_loc = glGetUniformLocation(program, "light.ambient_val");
        ambient_loc = glGetUniformLocation(program, "material.ambient");
        diffuse_loc = glGetUniformLocation(program, "material.diffuse");
        specular_loc = glGetUniformLocation(program, "material.specular");
        shininess_loc = glGetUniformLocation(program, "material.shininess");
    }

    void drag(float dx, float dy) {
        phi_deg += dx * 0.1f;
        theta_deg += dy * 0.1f;
        theta_deg = theta_deg > 90.0 ? 90.0 : theta_deg;
        theta_deg = theta_deg < -90.0 ? -90.0 : theta_deg;
        update();
    }

    void zoom(float dy) {
        float ratio = fd / 100.f;
        fd += dy * ratio;
        if (fd < 0.1) fd = 0.1f;
        update();
    }

    void dolly(float dy) {
        float ratio = od / 100.f;
        od -= dy * ratio;  // note: we go in the opposite direction of zoooming
        if (od < 0.5) od = 0.5f;
        update();
    }

    void view_tele() {
        fd = tele_fd;
        od = tele_fd;
        update();
    }

    void view_normal() {
        fd = normal_fd;
        od = normal_fd;
        update();
    }

    void view_wide() {
        fd = wide_fd;
        od = wide_fd;
        update();
    }

    void push_update() const { update(); }

   private:
    void update() const {
        float ncp = od - 1.f;  // distance near clip plane
        if (ncp < 0.1) ncp = 0.1f;
        float fcp = od + 1.f;  // distance far clip plane

        // prepare rotation matrices
        float ps = glm::sin(glm::radians(phi_deg));
        float pc = glm::cos(glm::radians(phi_deg));
        glm::mat4 ry(pc, 0.0, -ps, 0.0,   // 1st column
                     0.0, 1.0, 0.0, 0.0,  // 2nd column
                     ps, 0.0, pc, 0.0,    // 3rd column
                     0.0, 0.0, 0.0, 1.0);

        float ts = glm::sin(glm::radians(theta_deg));
        float tc = glm::cos(glm::radians(theta_deg));
        glm::mat4 rx(1.0, 0.0, 0.0, 0.0,  // 1st column
                     0.0, tc, ts, 0.0,    // 2nd column
                     0.0, -ts, tc, 0.0,   // 3rd column
                     0.0, 0.0, 0.0, 1.0);

        // prepare translation matrix
        glm::mat4 tz(1.0, 0.0, 0.0, 0.0,  // 1st column
                     0.0, 1.0, 0.0, 0.0,  // 2nd column
                     0.0, 0.0, 1.0, 0.0,  // 3rd column
                     0.0, 0.0, -od, 1.0   // translate object along the Z axis
        );

        // Combine into View matrix V
        glm::mat4 V = tz * rx * ry;

        // prepare projection matrix P
        float a = (fcp + ncp) / (ncp - fcp);      // coefficient 3rd col
        float b = 2.f * fcp * ncp / (ncp - fcp);  // coefficient 4th col

        glm::mat4 P(fd, 0.0, 0.0, 0.0,  // 1st column
                    0.0, fd, 0.0, 0.0,  // 2nd column
                    0.0, 0.0, a, -1.0,  // 3rd column
                    0.0, 0.0, b, 0.0    // 4th column
        );

        // Compute VP matrix
        glm::mat4 vp;
        vp = P * V;
        glUniformMatrix4fv(tm_loc, 1, GL_FALSE, &vp[0][0]);

        // Calculate Camera position in WC: inverse(V) * Origin
        glm::mat4 invV = glm::inverse(V);
        glm::vec4 cam_pos4 = invV * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 cam_pos(cam_pos4.x, cam_pos4.y, cam_pos4.z);

        // Position the light exactly where the camera is located
        glm::vec3 light_pos = cam_pos;

        // Camera position
        glUniform3fv(camera_pos_loc, 1, &cam_pos[0]);

        // Light parameters
        glUniform3fv(direct_pos_loc, 1, &light_pos[0]);
        glUniform3fv(direct_val_loc, 1, &direct_val[0]);
        glUniform3fv(ambient_val_loc, 1, &ambient_val[0]);

        // Push generic material parameters
        glUniform3fv(ambient_loc, 1, &ambient[0]);
        glUniform3fv(diffuse_loc, 1, &diffuse[0]);
        glUniform3fv(specular_loc, 1, &specular[0]);
        glUniform1f(shininess_loc, shininess);
    }
};

class Scene {
   private:
    // data to be drawn
    std::vector<float> points;
    std::vector<unsigned int> indices;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;

   public:
    explicit Scene(const std::string& filename) { load(filename); }
    ~Scene() { clean(); }

    void load(const std::string& filename) {
        Mesh mesh(filename);
        mesh.pack4gpu(points, indices);
        send_arrays_2a3f();
    }

    void clean() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void draw() const {
        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw all elements as described by indices
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT,
                       nullptr);
    }

   private:
    // send to the gpu the mesh arrays:
    // - the mesh vertices, 2 attributes, 3 floats each
    // - the mesh indices
    void send_arrays_2a3f() {
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Attribute 1: normal (nx, ny, nz)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ebo);
        // MUST be bound after the VAO's binding!
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    }
};

/////////////
// Helpers //
/////////////

void reload_shaders(const std::string& vertex, const std::string& fragment, Shaders& shaders,
                    CameraLights& camera) {
    shaders.reload(vertex, fragment);
    shaders.use();
    camera.update_locations(shaders.program);
    camera.push_update();
}

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, CameraLights& camera,
            bool& running) {
    static std::string current_vertex = gouraud_vertex_shader_path;
    static std::string current_fragment = gouraud_fragment_shader_path;

    bool should_reload = false;

    switch (key.scancode) {
        using enum sf::Keyboard::Scancode;

        // --- Shaders ---
        case F:
            current_vertex = flat_vertex_shader_path;
            current_fragment = flat_fragment_shader_path;
            should_reload = true;
            break;
        case P:
            current_vertex = phong_vertex_shader_path;
            current_fragment = phong_fragment_shader_path;
            should_reload = true;
            break;
        case G:
            current_vertex = gouraud_vertex_shader_path;
            current_fragment = gouraud_fragment_shader_path;
            should_reload = true;
            break;
        case Space:
            should_reload = true;
            break;

        // --- Camera ---
        case N:
            camera.view_normal();
            break;
        case T:
            camera.view_tele();
            break;
        case W:
            camera.view_wide();
            break;

        // --- Window ---
        case Escape:
            running = false;
            break;

        default:
            break;
    }

    if (should_reload) {
        reload_shaders(current_vertex, current_fragment, shaders, camera);
    }
}

void handle(const sf::Event::MouseMoved* mouse, CameraLights& camera) {
    auto x = static_cast<float>(mouse->position.x);
    auto y = static_cast<float>(mouse->position.y);
    static float prev_x = 0;
    static float prev_y = 0;

    float dx = x - prev_x;
    float dy = y - prev_y;

    prev_x = x;
    prev_y = y;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        camera.drag(dx, dy);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        camera.zoom(dy);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt))
        camera.dolly(dy);
}

//////////
// Main //
//////////

int main(int argc, char* argv[]) {
    // mandatory command line argument: mesh file to open
    std::string meshfile = "";
    if (argc > 1)
        meshfile = argv[1];
    else {
        std::cout << "Usage: " << argv[0] << " meshfile\n";
        exit(1);
    }

    // Startup //

    Setup setup;
    sf::Window& window = *setup.window;

    Shaders shaders(gouraud_vertex_shader_path, gouraud_fragment_shader_path);
    shaders.use();

    CameraLights camera(shaders);
    Scene scene(meshfile);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    // Main Loop //

    bool running = true;
    while (running) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle(*key_pressed, shaders, camera, running);
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                handle(mouse, camera);
        }

        scene.draw();
        window.display();
    }

    return 0;
}
