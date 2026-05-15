#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) uniform float phi_deg;
layout(location = 3) uniform float theta_deg;

out vec3 interpolated_color;

void main() {
    float phi = radians(phi_deg);
    float theta = radians(theta_deg);

    float cp = cos(phi);
    float sp = sin(phi);
    float ct = cos(theta);
    float st = sin(theta);

    // Matrice di rotazione asse Y (phi)
    mat4 Ry = mat4(cp, 0.0, -sp, 0.0, 0.0, 1.0, 0.0, 0.0, sp, 0.0, cp, 0.0, 0.0, 0.0, 0.0, 1.0);

    // Matrice di rotazione asse X (theta)
    mat4 Rx = mat4(1.0, 0.0, 0.0, 0.0, 0.0, ct, st, 0.0, 0.0, -st, ct, 0.0, 0.0, 0.0, 0.0, 1.0);

    // Matrice di traslazione lungo Z (con centro z = -2.0)
    mat4 T = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -2.0, 1.0);

    // Matrice di Proiezione
    float fd = 2.0;
    float f_cp = 3.0;
    float ncp = 1.0;

    float A = -(f_cp + ncp) / (f_cp - ncp);
    float B = -(2.0 * f_cp * ncp) / (f_cp - ncp);

    mat4 P = mat4(fd, 0.0, 0.0, 0.0, 0.0, fd, 0.0, 0.0, 0.0, 0.0, A, -1.0, 0.0, 0.0, B, 0.0);

    mat4 MVP = P * T * Rx * Ry;

    gl_Position = MVP * vec4(position, 1.0);
    interpolated_color = vertex_color;
}
