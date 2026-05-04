#version 430 core

layout(location = 0) uniform vec3 foreground;
layout(location = 0) out vec4 frag_color;

void main() {
    frag_color = vec4(foreground, 1.0);
}
