#pragma once

inline const char* fragmentShader = R""""(

#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(-0.5, -0.5, -0.5));  // Directional light
uniform vec3 baseColor = vec3(0.8, 0.3, 0.3);
uniform float minBrightness = 0.05;
uniform float maxBrightness = 1.1;

void main() {
    vec3 norm = normalize(Normal);
    float diff = 0.5f+(dot(norm, -lightDir))*0.5f; // Lambertian diffuse
    vec3 color = baseColor * mix(minBrightness, maxBrightness, diff);
    FragColor = vec4(color, 1.0);
}

)"""";