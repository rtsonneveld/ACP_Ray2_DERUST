#pragma once

namespace Shaders {

  namespace Basic {
    inline const char* Fragment = R""""(

#version 330 core
in vec3 Normal;
in vec3 FaceNormal;
in vec3 FragPos;
in vec3 Bary; // From vertex shader

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(-0.5, -0.5, -0.5));
uniform vec4 baseColorFloors = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 baseColorWalls = vec4(0.5, 0.5, 0.5, 0.5);
uniform vec4 minBrightness = vec4(0.05, 0.05, 0.05, 1.0);
uniform vec4 maxBrightness = vec4(1.1, 1.1, 1.1, 1.0);

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float uAlphaMult = 1.0;
uniform bool transparentWalls = false;

uniform float wireThickness = 1.5;

uniform uint uCollisionFlags;
uniform vec2 uScreenSize = vec2(1000.0,1000.0);

// --- Edge factor for wireframe ---
float edgeFactor() {
    vec3 d = fwidth(Bary);
    vec3 a3 = smoothstep(vec3(0.0), d * wireThickness, Bary);
    return min(min(a3.x, a3.y), a3.z);
}

// --- Collect active colors ---
int getCollisionColors(uint flags, out vec4 colors[16]) {
    int count = 0;
    if ((flags & (1u << 0u))  != 0u) colors[count++] = vec4(0.0, 0.2, 1.0, 1.0);
    if ((flags & (1u << 1u))  != 0u) colors[count++] = vec4(0.8, 0.8, 0.1, 1.0);
    if ((flags & (1u << 2u))  != 0u) colors[count++] = vec4(0.0, 0.8, 0.0, 1.0);
    if ((flags & (1u << 4u))  != 0u) colors[count++] = vec4(1.0, 0.0, 1.0, 1.0);
    if ((flags & (1u << 5u))  != 0u) colors[count++] = vec4(0.59, 0.29, 0.0, 1.0);
    if ((flags & (1u << 6u))  != 0u) colors[count++] = vec4(0.59, 0.29, 0.0, 1.0);
    if ((flags & (1u << 7u))  != 0u) colors[count++] = vec4(0.5, 0.8, 1.0, 1.0);
    if ((flags & (1u << 8u))  != 0u) colors[count++] = vec4(0.4, 0.0, 0.0, 1.0);
    if ((flags & (1u << 9u))  != 0u) colors[count++] = vec4(0.6, 0.5, 0.0, 1.0);
    if ((flags & (1u << 10u)) != 0u) colors[count++] = vec4(1.0, 0.4, 0.0, 1.0);
    if ((flags & (1u << 11u)) != 0u) colors[count++] = vec4(1.0, 0.0, 0.0, 1.0);
    if ((flags & (1u << 12u)) != 0u) colors[count++] = vec4(1.0, 0.0, 1.0, 1.0);
    if ((flags & (1u << 13u)) != 0u) colors[count++] = vec4(1.0, 0.0, 1.0, 1.0);
    if ((flags & (1u << 14u)) != 0u) colors[count++] = vec4(0.25, 1.0, 0.9, 1.0);
    if ((flags & (1u << 15u)) != 0u) colors[count++] = vec4(0.0, 0.0, 0.0, 0.0);
    return count;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 faceNorm = normalize(FaceNormal);
    vec3 interpNorm = mix(norm, faceNorm, 0.5);
    float diff = 0.5f + (dot(interpNorm, -lightDir)) * 0.5f;

    FragColor = baseColorFloors;
    if (transparentWalls && faceNorm.z < 0.70710678) {
        FragColor = baseColorWalls;
    }

    FragColor *= uColor;
    FragColor *= mix(minBrightness, maxBrightness, diff);
    FragColor.a *= uAlphaMult;

    vec4 colors[16];
    int colorCount = getCollisionColors(uCollisionFlags, colors);
    if (colorCount > 0 && uScreenSize.x > 0.0 && uScreenSize.y > 0.0) {
        vec2 screenUV = gl_FragCoord.xy / uScreenSize;
        float scale = 100.0;
        float lineIndex = floor(mod((screenUV.x + screenUV.y) * scale, float(colorCount)));
        int idx = int(lineIndex);
        FragColor *= colors[idx];
    }

    float edge = edgeFactor();
    FragColor = mix(vec4(FragColor.rgb * 0.5, 1.0), FragColor, edge);
}

)"""";


    inline const char* Vertex = R""""(

      #version 330 core
      layout(location = 0) in vec3 aPos;
      layout(location = 1) in vec3 aNormal;
      layout(location = 2) in vec3 aFaceNormal;
      layout(location = 3) in vec3 aBary;   // New attribute

      uniform mat4 uModel;
      uniform mat4 uView;
      uniform mat4 uProjection;

      out vec3 Normal;
      out vec3 FaceNormal;
      out vec3 FragPos;
      out vec3 Bary; // Pass to fragment

      void main() {
        FragPos = vec3(uModel * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(uModel))) * aNormal;
        FaceNormal = mat3(transpose(inverse(uModel))) * aFaceNormal;
        Bary = aBary;
        gl_Position = uProjection * uView * vec4(FragPos, 1.0);
      }

  )"""";

  }

}