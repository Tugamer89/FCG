#version 430 core

in vec3 interpolated_color;

layout(location = 0) out vec4 fragment_color;

void main() {
    fragment_color = vec4(interpolated_color, 1.0);
}
