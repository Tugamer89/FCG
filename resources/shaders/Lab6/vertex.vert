#version 410 core

layout(location = 0) in vec3 vp; // Vertex Position
layout(location = 1) in vec3 vn; // Vertex Normal

// Coordinate and matrix uniforms
uniform mat4 tm; // MVP Matrix
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

// Output to fragment shader
out vec4 front_color;

void main() {
    // Standard projection onto screen
    gl_Position = tm * vec4(vp, 1.0);

    // Geometry vectors
    vec3 N = normalize(vn); 
    vec3 L = normalize(light_pos - vp); 
    vec3 V = normalize(cam_pos - vp);   
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
    front_color = vec4(result, 1.0);
}
