#pragma once

namespace Shaders {

  namespace Basic {

    inline const char* Fragment = R""""(

#version 430 core
in vec3 Normal;
in vec3 FaceNormal;
in vec3 FragPos;
in vec3 Bary; // From vertex shader

// Two render targets for WOIT
layout(location = 0) out vec4 accum;
layout(location = 1) out float revealage;

uniform vec3 lightDir = normalize(vec3(-0.5, -0.5, -0.5));
uniform vec4 baseColorFloors = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 baseColorWalls = vec4(0.5, 0.5, 0.5, 0.5);
uniform vec4 minBrightness = vec4(0.05, 0.05, 0.05, 1.0);
uniform vec4 maxBrightness = vec4(1.1, 1.1, 1.1, 1.0);

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float uAlphaMult = 1.0;
uniform bool transparentWalls = false;
uniform bool uOpaquePass = false;

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
    if ((flags & (1u << 0u))  != 0u) colors[count++] = vec4(0.0, 0.2, 1.0, 1.0); // Slide
    if ((flags & (1u << 1u))  != 0u) colors[count++] = vec4(0.8, 0.8, 0.1, 1.0); // Trampoline
    if ((flags & (1u << 2u))  != 0u) colors[count++] = vec4(0.0, 0.8, 0.0, 1.0); // Ledge
    //if ((flags & (1u << 3u))  != 0u) colors[count++] = vec4(0.0, 0.8, 0.0, 1.0); // User1
    if ((flags & (1u << 4u))  != 0u) colors[count++] = vec4(0.0, 1.0, 1.0, 1.0); // GrappinBis (unused)
    if ((flags & (1u << 5u))  != 0u) colors[count++] = vec4(0.59, 0.29, 0.0, 1.0); // Hangable ceiling
    if ((flags & (1u << 6u))  != 0u) colors[count++] = vec4(0.59, 0.29, 0.0, 1.0); // Climbable wall
    if ((flags & (1u << 7u))  != 0u) colors[count++] = vec4(0.5, 0.8, 1.0, 1.0); // Electric
    if ((flags & (1u << 8u))  != 0u) colors[count++] = vec4(0.4, 0.0, 0.0, 1.0); // Lava deathwarp
    if ((flags & (1u << 9u))  != 0u) colors[count++] = vec4(0.6, 0.5, 0.0, 1.0); // Fall trigger
    if ((flags & (1u << 10u)) != 0u) colors[count++] = vec4(1.0, 0.4, 0.0, 1.0); // Hurt trigger
    if ((flags & (1u << 11u)) != 0u) colors[count++] = vec4(1.0, 0.0, 0.0, 1.0); // Deathwarp
    if ((flags & (1u << 12u)) != 0u) colors[count++] = vec4(1.0, 0.0, 1.0, 1.0); // User2
    if ((flags & (1u << 13u)) != 0u) colors[count++] = vec4(1.0, 0.0, 1.0, 1.0); // User3
    if ((flags & (1u << 14u)) != 0u) colors[count++] = vec4(0.25, 1.0, 0.9, 0.65); // Water
    if ((flags & (1u << 15u)) != 0u) colors[count++] = vec4(0.0, 0.0, 0.0, 0.0); // NoCollision
    return count;
}

void main() {
    // --- shading & base color ---
    vec3 norm = normalize(Normal);
    vec3 faceNorm = normalize(FaceNormal);
    vec3 interpNorm = mix(norm, faceNorm, 0.5);
    float diff = 0.5 + (dot(interpNorm, -lightDir)) * 0.5;

    vec4 out_col = baseColorFloors;
    if (transparentWalls && faceNorm.z < 0.70710678) {
        out_col = baseColorWalls;
    }

    out_col *= uColor;
    out_col *= mix(minBrightness, maxBrightness, diff);
    out_col.a *= uAlphaMult;

    // --- collision color striping (multiplicative) ---
    vec4 colors[16];
    int colorCount = getCollisionColors(uCollisionFlags, colors);
    if (colorCount > 0 && uScreenSize.x > 0.0 && uScreenSize.y > 0.0) {
        vec2 screenUV = gl_FragCoord.xy / uScreenSize;
        float scale = 100.0;
        float lineIndex = floor(mod((screenUV.x + screenUV.y) * scale, float(colorCount)));
        int idx = int(lineIndex);
        out_col *= colors[idx];
    }

    // --- wireframe edge anti-aliasing ---
    float edge = edgeFactor();
    out_col = mix(vec4(out_col.rgb * 0.5, out_col.a), out_col, edge);

    // --- Opaque/Transparent pass handling ---
    if (uOpaquePass) {
        // --- OPAQUE PASS ---
        // Only keep fully opaque fragments
        if (out_col.a < 0.999) discard;

        accum = vec4(out_col.rgb, 1.0);
        return;
    }

    // --- TRANSPARENT PASS (Weighted Blended OIT) ---
    // Skip fully opaque fragments
    if (out_col.a >= 0.999) discard;

    float csz = abs(1.0 / gl_FragCoord.w);

    float factor = 1.0 / 200.0;
    float z = factor * csz;
    float weight = clamp((0.03 / (1e-5 + pow(z, 4.0))), 1e-4, 3e3);

    // --- write MRTs required by WOIT ---
    accum = vec4(out_col.rgb * out_col.a, out_col.a) * weight;
    revealage = out_col.a;
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