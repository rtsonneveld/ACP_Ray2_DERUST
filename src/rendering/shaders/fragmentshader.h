#pragma once

inline const char* fragmentShader = R""""(

#version 330 core
out vec4 FragColor;
in float depth;

void main() {
    float shade = clamp(1.0 - depth / 50.0, 0.0, 1.0);  // Adjust 50.0 as needed
    vec3 baseColor = vec3(0.8, 0.3, 0.3);
    FragColor = vec4(baseColor * shade, 1.0);
}

)"""";