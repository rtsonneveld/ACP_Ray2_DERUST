#pragma once

inline const char* vertexShader = R""""(

#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float depth;

void main() {\
  vec4 viewPos = view * model * vec4(aPos, 1.0);
  depth = -viewPos.z;  // Camera-space depth (positive values in front of camera)
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}

)"""";