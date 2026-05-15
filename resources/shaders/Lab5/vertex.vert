#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

uniform mat4 vp;

out vec3 interpolated_color;

void main() {
    gl_Position = vp * vec4(pos, 1.0);
    interpolated_color = col;
}
