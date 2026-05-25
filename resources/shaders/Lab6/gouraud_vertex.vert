#version 410 core

layout(location = 0) in vec3 vp;  // Vertex Position
layout(location = 1) in vec3 vn;  // Vertex Normal

uniform mat4 tm;  // MVP Matrix
uniform vec3 camera_pos;

struct Light {
    vec3 direct_pos;
    vec3 direct_val;
    vec3 ambient_val;
};
uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

out vec4 front_color;

void main() {
    // Standard projection onto screen
    gl_Position = tm * vec4(vp, 1.0);

    // Geometry vectors
    vec3 N = normalize(vn);
    vec3 L = normalize(light.direct_pos - vp);
    vec3 V = normalize(camera_pos - vp);
    vec3 R = reflect(-L, N);

    // Ambient reflection
    vec3 ambient = light.ambient_val * material.ambient;

    // Diffuse reflection
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light.direct_val * material.diffuse * diff;

    // Specular reflection
    float spec = 0.0;
    if (diff > 0.0) {
        spec = pow(max(dot(V, R), 0.0), material.shininess);
    }
    vec3 specular = light.direct_val * material.specular * spec;

    // Combine terms
    vec3 result = clamp(ambient + diffuse + specular, 0.0, 1.0);
    front_color = vec4(result, 1.0);
}
