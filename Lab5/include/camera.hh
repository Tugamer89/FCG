#ifndef CAMERA_HH
#define CAMERA_HH

#include <SFML/Window.hpp>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

class Camera {
   private:
    static constexpr float DEFAULT_ZOOM = 2.f;

    glm::mat4 vp;
    float phi_deg = 0;
    float theta_deg = 0;
    float zoom_value = DEFAULT_ZOOM;
    float dolly_value = -DEFAULT_ZOOM;
    sf::Vector2f move_value = {0, 0};

    GLint vp_location;

    void update() {
        float phi = glm::radians(phi_deg);
        float theta = glm::radians(theta_deg);

        float cp = cos(phi);
        float sp = sin(phi);
        float ct = cos(theta);
        float st = sin(theta);

        // Y-axis rotation matrix (phi)
        glm::mat4 Ry(  //
            cp, 0.0, -sp, 0.0,     //
            0.0, 1.0, 0.0, 0.0,    //
            sp, 0.0, cp, 0.0,      //
            0.0, 0.0, 0.0, 1.0     //
        );

        // X-axis rotation matrix (theta)
        glm::mat4 Rx(  //
            1.0, 0.0, 0.0, 0.0,    //
            0.0, ct, st, 0.0,      //
            0.0, -st, ct, 0.0,     //
            0.0, 0.0, 0.0, 1.0     //
        );

        // TODO: base translation to the center of the model (e.g., by using the bounding box) when loading new mesh

        // Translation matrix along Z (center z = -2.0)
        glm::mat4 T(                          //
            1.0, 0.0, 0.0, 0.0,                           //
            0.0, 1.0, 0.0, 0.0,                           //
            0.0, 0.0, 1.0, 0.0,                           //
            move_value.x, move_value.y, dolly_value, 1.0  //
        );

        // Projection Matrix
        float fd = zoom_value;
        float f_cp = 3.0;
        float n_cp = 1.0;

        float A = -(f_cp + n_cp) / (f_cp - n_cp);
        float B = -(2.0f * f_cp * n_cp) / (f_cp - n_cp);

        glm::mat4 P(fd, 0.0, 0.0, 0.0,  //
                                0.0, fd, 0.0, 0.0,  //
                                0.0, 0.0, A, -1.0,  //
                                0.0, 0.0, B, 0.0);

        vp = P * T * Rx * Ry;

        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(vp));
    }

   public:
    explicit Camera(GLuint shader_program) : vp_location(glGetUniformLocation(shader_program, "vp")) {
        update();
    }

    void drag(sf::Vector2f dpos) {
        const float scale = 0.1f;

        phi_deg += dpos.x * scale;
        theta_deg += dpos.y * scale;

        theta_deg = std::clamp(theta_deg, -90.f, 90.f);

        update();
    }

    void zoom(float offset) {
        zoom_value += offset * 0.01f;
        zoom_value = std::clamp(zoom_value, 0.001f, 20.f);

        update();
    }

    void dolly(float offset) {
        dolly_value += offset * 0.01f;
        update();
    }

    void move(sf::Vector2f dpos) {
        dpos.y = -dpos.y;
        move_value += dpos * 0.005f;
        update();
    }

    void reset() {
        phi_deg = 0;
        theta_deg = 0;
        zoom_value = DEFAULT_ZOOM;
        dolly_value = -DEFAULT_ZOOM;
        move_value = {0, 0};
        update();
    }

    void updateAnimation(sf::Time dt) {
        const float rotation_speed = 20.f;
        static float theta_dir = 1;

        if (theta_deg > 90.f)
            theta_dir = -1;
        else if (theta_deg < -90.f)
            theta_dir = 1;

        phi_deg += rotation_speed * dt.asSeconds();
        theta_deg += theta_dir * rotation_speed * dt.asSeconds() * 0.5f;

        update();
    }
};

#endif
