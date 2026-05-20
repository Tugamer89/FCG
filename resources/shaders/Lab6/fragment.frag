#version 410 core

in vec3 interpolated_normal;
out vec4 fragment_color;

void main() {
    fragment_color = vec4(interpolated_normal * 0.5 + 0.5, 1.0);
}
