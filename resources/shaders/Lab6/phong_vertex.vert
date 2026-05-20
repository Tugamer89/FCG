#version 410 core

layout(location = 0) in vec3 vp;  // Vertex Position
layout(location = 1) in vec3 vn;  // Vertex Normal

uniform mat4 tm;  // MVP Matrix

// Output to fragment shader
out vec3 vertex_normal;
out vec3 vertex_position;

void main() {
    gl_Position = tm * vec4(vp, 1.0);

    vertex_normal = vn;
    vertex_position = vp;
}
