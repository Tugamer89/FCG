#version 410 core

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 vn;
uniform mat4 tm;

out vec3 interpolated_normal;

void main() {
    vec4 p;
    p = vec4(vp, 1.0);
    gl_Position = tm * p;
    interpolated_normal = vn;
}
