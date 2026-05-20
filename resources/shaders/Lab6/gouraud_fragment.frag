#version 410 core

in vec4 front_color;

out vec4 frag_colour;

void main() {
    frag_colour = front_color;
}
