#version 410 core

in vec3 vertex_position;

// Coordinate and matrix uniforms
uniform vec3 light_pos;
uniform vec3 cam_pos;

// Material properties
uniform vec3 mat_diffuse;
uniform vec3 mat_specular;
uniform float mat_shininess;
uniform vec3 mat_ambient;

// Lighting properties
uniform vec3 light_color;
uniform vec3 ambient_color;

out vec4 frag_colour;

void main() {
    vec3 dx = dFdx(vertex_position);
    vec3 dy = dFdy(vertex_position);
    vec3 vertex_normal = cross(dx, dy);

    // Geometry vectors
    vec3 N = normalize(vertex_normal);
    vec3 L = normalize(light_pos - vertex_position);
    vec3 V = normalize(cam_pos - vertex_position);
    vec3 R = reflect(-L, N);

    // Ambient reflection
    vec3 ambient = ambient_color * mat_ambient;

    // Diffuse reflection
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light_color * mat_diffuse * diff;

    // Specular reflection
    float spec = 0.0;
    if (diff > 0.0) {
        spec = pow(max(dot(V, R), 0.0), mat_shininess);
    }
    vec3 specular = light_color * mat_specular * spec;

    // Combine terms
    vec3 result = clamp(ambient + diffuse + specular, 0.0, 1.0);
    frag_colour = vec4(result, 1.0);
}
