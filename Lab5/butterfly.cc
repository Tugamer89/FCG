#include <SFML/Window.hpp>

#include "include/camera.hh"
#include "include/hotshaders.hh"
#include "include/mesh.hh"
#include "include/scene.hh"
#include "include/setup.hh"

const char* const vertLoc = "resources/shaders/Lab5/vertex.vert";
const char* const fragLoc = "resources/shaders/Lab5/fragment.frag";
const char* const bunnyMeshLoc = "resources/meshes/bunny.off";
const char* const pyramidMeshLoc = "resources/meshes/pyramid.off";

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, Camera& camera, Scene& scene,
            bool& running, bool& animate) {
    switch (key.scancode) {
        using enum sf::Keyboard::Scancode;
        case Space:
            shaders.reload(vertLoc, fragLoc);
            glUseProgram(shaders.program);
            scene.reload(bunnyMeshLoc);
        case R:
            camera.reset();
            return;
        case Escape:
            running = false;
            return;
        case Enter:
            animate = !animate;
            return;
        default:
            return;
    }
}

void handle(const sf::Event::MouseMoved& mouse, Camera& camera, bool& just_entered) {
    sf::Vector2f mouse_pos(mouse.position);
    static sf::Vector2f prev_pos = mouse_pos;

    if (just_entered) {
        prev_pos = mouse_pos;
        just_entered = false;
        return;
    }

    sf::Vector2f dpos = prev_pos - mouse_pos;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
        camera.zoom(dpos.y);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
        camera.dolly(dpos.y);
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        camera.drag(dpos);
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
        camera.move(dpos);
    }

    prev_pos = mouse_pos;
}

//////////
// Main //
//////////

int main() {
    // setup SFML + OpenGL, using GLAD
    Setup setup;
    sf::Window& window = setup.window;

    Shaders shaders(vertLoc, fragLoc);
    shaders.use();

    Scene scene(shaders.program);
    Camera camera(shaders.program);

    // face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // depth testing
    glEnable(GL_DEPTH_TEST);

    ///////////////
    // Main Loop //
    ///////////////

    bool animate = false;
    bool just_entered = true;
    bool running = true;
    sf::Clock clock;

    while (running) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (event->is<sf::Event::MouseEntered>())
                just_entered = true;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle(*key_pressed, shaders, camera, scene, running, animate);
            else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
                handle(*mouse, camera, just_entered);
        }

        sf::Time dt = clock.restart();

        if (animate) {
            camera.updateAnimation(dt);
        }

        scene.draw();
        window.display();
    }

    return 0;
}
