#pragma once

inline const char* vertexShader = R""""(

#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aFaceNormal;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 Normal;
out vec3 FaceNormal;
out vec3 FragPos;

void main() {\
  FragPos = vec3(uModel * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(uModel))) * aNormal;
  FaceNormal = mat3(transpose(inverse(uModel))) * aFaceNormal;
  gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}

)"""";