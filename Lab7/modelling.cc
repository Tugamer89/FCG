#define GLAD_GL_IMPLEMENTATION
#include <SFML/Window.hpp>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

#include "glad/gl.h"
#include "include/hotshaders.hh"
#include "include/matrices.hh"
#include "include/mesh.hh"

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

class Lights {
   public:
    glm::vec3 light_direct_pos = {2.0, 2.0, 0.0};   // xyz
    glm::vec3 light_direct_val = {1.0, 1.0, 1.0};   // rgb
    glm::vec3 light_ambient_val = {0.1, 0.1, 0.1};  // rgb
    glm::vec3 material_diffuse = {0.8, 0.7, 0.6};   // rgb
    glm::vec3 material_ambient = {0.5, 0.5, 0.8};   // rgb
    glm::vec3 material_specular = {1.0, 1.0, 1.0};  // rgb
    float material_shininess = 1000.f;              // scalar

   private:
    // lights and materials
    GLint light_direct_pos_loc;    // xyz
    GLint light_direct_val_loc;    // rgb
    GLint light_ambient_val_loc;   // rgb
    GLint material_diffuse_loc;    // rgb
    GLint material_ambient_loc;    // rgb
    GLint material_specular_loc;   // rgb
    GLint material_shininess_loc;  // scalar

   public:
    Lights(Shaders& shaders) { locations(shaders); }

    void locations(Shaders& shaders) {
        light_direct_pos_loc = glGetUniformLocation(shaders.program, "light.direct_pos");
        light_direct_val_loc = glGetUniformLocation(shaders.program, "light.direct_val");
        light_ambient_val_loc = glGetUniformLocation(shaders.program, "light.ambient_val");
        material_diffuse_loc = glGetUniformLocation(shaders.program, "material.diffuse");
        material_ambient_loc = glGetUniformLocation(shaders.program, "material.ambient");
        material_specular_loc = glGetUniformLocation(shaders.program, "material.specular");
        material_shininess_loc = glGetUniformLocation(shaders.program, "material.shininess");
    }

    void parameters() {
        glUniform3fv(light_direct_val_loc, 1, &light_direct_val[0]);
        glUniform3fv(light_ambient_val_loc, 1, &light_ambient_val[0]);
        glUniform3fv(material_diffuse_loc, 1, &material_diffuse[0]);
        glUniform3fv(material_ambient_loc, 1, &material_ambient[0]);
        glUniform3fv(material_specular_loc, 1, &material_specular[0]);
        glUniform1fv(material_shininess_loc, 1, &material_shininess);
    }

    void position(glm::mat4& inverse_view_matrix) {
        glm::vec4 ldp4 = glm::vec4(light_direct_pos, 1.0);
        ldp4 = inverse_view_matrix * ldp4;
        glm::vec3 ldp3 = {ldp4.x, ldp4.y, ldp4.z};
        glUniform3fv(light_direct_pos_loc, 1, &ldp3[0]);
    }

   private:
};

class Camera {
   public:
    glm::mat4 v;
    glm::mat4 inv_v;
    glm::mat4 vp;

   private:
    float phi_deg = 0.0;
    float theta_deg = 0.0;

    const float normal_fd = 4.0;
    const float tele_fd = 100.0;
    const float wide_fd = 1.5;

    float fd;  // focal distance
    float od;  // object distance

    GLint camera_pos_loc;                    // xyz
    glm::vec3 camera_pos = {0.0, 0.0, 0.0};  // xyz

   public:
    Camera(Shaders& shaders) { locations(shaders); }

    void locations(Shaders& shaders) {
        camera_pos_loc = glGetUniformLocation(shaders.program, "camera_pos");
    }

    void drag(float dx, float dy) {
        phi_deg += dx * 0.1;
        theta_deg += dy * 0.1;
        theta_deg = theta_deg > 90.0f ? 90.0f : theta_deg;
        theta_deg = theta_deg < -90.0f ? -90.0f : theta_deg;
        projection();
    }

    void zoom(float dy) {
        float ratio = fd / 100.0;
        fd += dy * ratio;
        if (fd < 0.1) fd = 0.1;
    }

    void dolly(float dy) {
        float ratio = od / 100.0;
        od -= dy * ratio;  // note: we go in the opposite direction of zoooming
        if (od < 0.5) od = 0.5;
        projection();
    }

    void view_tele() {
        fd = tele_fd;
        od = tele_fd;
        projection();
    }

    void view_normal() {
        fd = normal_fd;
        od = normal_fd;
        projection();
    }

    void view_wide() {
        fd = wide_fd;
        od = wide_fd;
        projection();
    }

    void projection() {
        float ncp = od - 1.0;  // distance near clip plane
        if (ncp < 0.1) ncp = 0.1;
        float fcp = od + 1.0;  // distance far clip plane

        // prepare rotation matrices
        //// Convert degrees to radians, compute sin and cos
        float ps = glm::sin(glm::radians(phi_deg));
        float pc = glm::cos(glm::radians(phi_deg));
        glm::mat4 ry(            //
            pc, 0.0, -ps, 0.0,   // 1st column
            0.0, 1.0, 0.0, 0.0,  // 2nd column
            ps, 0.0, pc, 0.0,    // 3rd column
            0.0, 0.0, 0.0, 1.0   //
        );

        // Convert degrees to radians, compute sin and cos
        float ts = glm::sin(glm::radians(theta_deg));
        float tc = glm::cos(glm::radians(theta_deg));
        glm::mat4 rx(            //
            1.0, 0.0, 0.0, 0.0,  // 1st column
            0.0, tc, ts, 0.0,    // 2nd column
            0.0, -ts, tc, 0.0,   // 3rd column
            0.0, 0.0, 0.0, 1.0   //
        );

        // prepare translation matrix
        glm::mat4 tz(            //
            1.0, 0.0, 0.0, 0.0,  // 1st column
            0.0, 1.0, 0.0, 0.0,  // 2nd column
            0.0, 0.0, 1.0, 0.0,  // 3rd column
            0.0, 0.0, -od, 1.0   // translate world along the Z axis
        );
        v = tz * rx * ry;

        inv_v = glm::inverse(v);

        // prepare projection matrix
        float a = (fcp + ncp) / (ncp - fcp);      // coefficient 3rd col
        float b = 2.0 * fcp * ncp / (ncp - fcp);  // coefficient 4th col

        /*** NOTE *******************************************************
         **  We use fd directly as coefficient in the first two lines. **
         **  It works because our scene is in a unitary cube.          **
         **  If the image plane is centered about the view axis, with  **
         **  width 2r and height 2t in view space, the coefficients    **
         **  containing fd must be scaled accordingly.                 **
         ****************************************************************/
        glm::mat4 pr(           //
            fd, 0.0, 0.0, 0.0,  // 1st column
            0.0, fd, 0.0, 0.0,  // 2nd column
            0.0, 0.0, a, -1.0,  // 3rd column
            0.0, 0.0, b, 0.0    // 4th column
        );

        // Compute VP matrix and update it
        vp = pr * v;
        inv_v = glm::inverse(v);

        glm::vec4 cp4 = {0.0, 0.0, 0.0, 1.0};
        cp4 = inv_v * cp4;
        glm::vec3 cp3 = {cp4.x, cp4.y, cp4.z};
        glUniform3fv(camera_pos_loc, 1, &cp3[0]);
    }
};

class GPUMesh {
   public:
    glm::vec3 center = {0.0, 0.0, 0.0};
    float extent = 1.0;

   private:
    std::vector<float> points = {};
    std::vector<unsigned int> indices = {};

    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    bool initialized = false;

   public:
    GPUMesh(std::string filename) { load(filename); }

    ~GPUMesh() { clean(); }

    void load(std::string filename) {
        Mesh mesh(filename);
        center = mesh.center;
        extent = mesh.extent;

        mesh.pack4gpu(points, indices);
        send_arrays_2a3f();
        initialized = true;
    }

    void clean() {
        if (initialized) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }
    }

    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

   protected:
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Attribute 1: 3 generic floats (u, v, w)
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

class Scene {
   public:
    Camera camera;
    Lights lights;
    GPUMesh mesh;
    glm::mat4 mesh_mm;

   private:
    GLint model_loc;
    GLint vp_loc;
    GLint tr_inv_model_loc;

   public:
    Scene(std::string meshfile, Shaders& shaders)
        : camera(shaders), lights(shaders), mesh(meshfile) {
        camera.view_normal();
        locations(shaders);
        update_all();
        mesh_mm = scaling(1.0 / mesh.extent) * translation(-mesh.center);
    }

    void locations(Shaders& shaders) {
        camera.locations(shaders);
        lights.locations(shaders);
        model_loc = glGetUniformLocation(shaders.program, "model");
        vp_loc = glGetUniformLocation(shaders.program, "vp");
        tr_inv_model_loc = glGetUniformLocation(shaders.program, "tr_inv_model");
    }

    void update_all() {
        camera.projection();
        lights.parameters();
        lights.position(camera.inv_v);
    }

    void draw() {
        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 rx90 = rotation_x(90.0f);
        glm::mat4 tr1 = translation(0, -0.9, 0);
        glm::mat4 full_mm = tr1 * rx90 * mesh_mm;  // full model matrix

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &full_mm[0][0]);
        glUniformMatrix4fv(vp_loc, 1, GL_FALSE, &camera.vp[0][0]);

        glm::mat3 tr_inv_model = glm::transpose(glm::inverse(glm::mat3(full_mm)));
        glUniformMatrix3fv(tr_inv_model_loc, 1, GL_FALSE, &tr_inv_model[0][0]);

        mesh.draw();
    }
};

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, Scene& scene, bool& running) {
    switch (key.scancode) {
        case sf::Keyboard::Scancode::G:
            shaders.reload("resources/shaders/Lab7/shader_gouraud.vert",
                           "resources/shaders/Lab7/shader_gouraud.frag");
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::P:
            shaders.reload("resources/shaders/Lab7/shader_phong.vert",
                           "resources/shaders/Lab7/shader_phong.frag");
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::F:
            shaders.reload("resources/shaders/Lab7/shader_flat.vert",
                           "resources/shaders/Lab7/shader_flat.frag");
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::C:
            shaders.reload("resources/shaders/Lab7/shader_normals.vert",
                           "resources/shaders/Lab7/shader_normals.frag");
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::N:
            scene.camera.view_normal();
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::T:
            scene.camera.view_tele();
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::W:
            scene.camera.view_wide();
            scene.update_all();
            return;
        case sf::Keyboard::Scancode::Escape:
            running = false;
            return;
        default:
            return;
    }
}

void handle(const sf::Event::MouseMoved* mouse, Scene& scene) {
    float x = mouse->position.x;
    float y = mouse->position.y;
    static float prev_x = 0.f;
    static float prev_y = 0.f;

    float dx = x - prev_x;
    float dy = y - prev_y;

    prev_x = x;
    prev_y = y;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        scene.camera.drag(dx, dy);
        scene.camera.projection();
        scene.lights.position(scene.camera.inv_v);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        scene.camera.zoom(dy);
        scene.camera.projection();
        scene.lights.position(scene.camera.inv_v);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
        scene.camera.dolly(dy);
        scene.camera.projection();
        scene.lights.position(scene.camera.inv_v);
    }
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

    //// Startup ////

    Setup setup;
    sf::Window& window = *setup.window;

    Shaders shaders("resources/shaders/Lab7/shader_flat.vert",
                    "resources/shaders/Lab7/shader_flat.frag");
    shaders.use();

    Scene scene(meshfile, shaders);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    //// Main Loop ////

    bool running = true;
    while (running) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle(*key_pressed, shaders, scene, running);
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                handle(mouse, scene);
        }

        scene.draw();
        window.display();
    }

    return 0;
}
