#version 410 core

layout(location = 0) in vec3 vp;  // Vertex Position
layout(location = 1) in vec3 vn;  // Vertex Normal -- note:

uniform mat4 tm;  // MVP Matrix

out vec3 vertex_position;

void main() {
    gl_Position = tm * vec4(vp, 1.0);

    vertex_position = vp;
}
