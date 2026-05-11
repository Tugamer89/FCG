#version 430 core

in vec3 color;

layout(location = 0) uniform vec3 mod_color;
layout(location = 0) out vec4 frag_color;

void main() {
    frag_color = vec4(color + mod_color, 1.0);
}
