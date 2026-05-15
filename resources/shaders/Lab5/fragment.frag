#version 410 core

const vec4 BLACK = vec4(0, 0, 0, 1);

in vec3 interpolated_color;
uniform bool draw_edge;

out vec4 fragment_color;

void main() {
    fragment_color = draw_edge ? BLACK : vec4(interpolated_color, 1);
}
