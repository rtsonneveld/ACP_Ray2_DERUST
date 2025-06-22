#pragma once

inline const char* fragmentShader = R""""(

#version 330 core
in vec3 Normal;
in vec3 FaceNormal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(-0.5, -0.5, -0.5));  // Directional light
uniform vec4 baseColorFloors = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 baseColorWalls = vec4(0.5, 0.5, 0.5, 0.5);
uniform vec4 minBrightness = vec4(0.05, 0.05, 0.05, 1.0);
uniform vec4 maxBrightness = vec4(1.1, 1.1, 1.1, 1.0);

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float uAlphaMult = 1.0;

void main() {
    
    vec3 norm = normalize(Normal);
    vec3 faceNorm = normalize(FaceNormal);
    vec3 interpNorm = mix(norm, faceNorm, 0.5);
    float diff = 0.5f+(dot(interpNorm, -lightDir))*0.5f; // Lambertian diffuse

    FragColor = baseColorFloors;

    if (faceNorm.z < 0.707106781187) { // 1/sqrt(2)
      //FragColor = baseColorWalls;
    }
    FragColor *= uColor;
    FragColor *= mix(minBrightness, maxBrightness, diff);
    FragColor.a *= uAlphaMult;
}

)"""";