#pragma once

inline const char* fragmentShader = R""""(

#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.8, 0.3, 0.3, 1.0);
}

)"""";