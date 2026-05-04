#version 430 core

layout(location = 0) in vec3 vp;

void main() {
    gl_Position = vec4(vp, 1.0);
}
