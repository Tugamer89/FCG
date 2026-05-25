#version 410 core

in vec3 vertex_normal;
in vec3 vertex_position;

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

out vec4 frag_colour;

void main() {
    // Geometry vectors
    vec3 N = normalize(vertex_normal);
    vec3 L = normalize(light.direct_pos - vertex_position);
    vec3 V = normalize(camera_pos - vertex_position);
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
    frag_colour = vec4(result, 1.0);
}
