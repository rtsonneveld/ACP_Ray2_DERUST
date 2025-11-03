#pragma once

namespace Shaders {

  namespace Basic {

    inline const char* Fragment = R""""(

#version 430 core
in vec3 Normal;
in vec3 FaceNormal;
in vec3 FragPos;
in vec3 Bary;
in vec3 Pos;
in vec3 ModelNormal;
in vec3 ModelFaceNormal;

layout(location = 0) out vec4 accum;
layout(location = 1) out float revealage;

uniform vec3 lightDir = normalize(vec3(-0.5, -0.5, -0.5));
uniform vec4 minBrightness = vec4(0.05, 0.05, 0.05, 1.0);
uniform vec4 maxBrightness = vec4(1.1, 1.1, 1.1, 1.0);

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float uAlphaMult = 1.0;
uniform bool uOpaquePass = false;
uniform bool transparentWalls = false;

layout(binding = 0) uniform sampler2D tex1;
layout(binding = 1) uniform sampler2D tex2;
uniform bool useSecondTexture = false;

uniform vec3 uvScale = vec3(1.0, 1.0, 1.0);
uniform float wireThickness = 1.5;
uniform vec2 uScreenSize = vec2(1000.0,1000.0);

// --- Edge factor for wireframe ---
float edgeFactor() {
    vec3 d = fwidth(Bary);
    vec3 a3 = smoothstep(vec3(0.0), d * wireThickness, Bary);
    return min(min(a3.x, a3.y), a3.z);
}

vec4 quantizedTriplanarTexture(sampler2D tex, vec3 pos, vec3 normal) {
    // Normalize normal
    vec3 n = normalize(normal);

    // Define the 14 discrete directions
    const vec3 dirs[14] = vec3[14](
        vec3( 1,  0,  0), vec3(-1,  0,  0), // ±X
        vec3( 0,  1,  0), vec3( 0, -1,  0), // ±Y
        vec3( 0,  0,  1), vec3( 0,  0, -1), // ±Z
        normalize(vec3( 1,  1,  1)), normalize(vec3(-1,  1,  1)),
        normalize(vec3( 1, -1,  1)), normalize(vec3(-1, -1,  1)),
        normalize(vec3( 1,  1, -1)), normalize(vec3(-1,  1, -1)),
        normalize(vec3( 1, -1, -1)), normalize(vec3(-1, -1, -1))
    );

    // Find best-matching direction (max dot)
    float bestDot = -1.0;
    int bestIndex = 0;
    for (int i = 0; i < 14; ++i) {
        float d = dot(n, dirs[i]);
        if (d > bestDot) {
            bestDot = d;
            bestIndex = i;
        }
    }

    vec3 bestDir = dirs[bestIndex];
    vec3 scaledPos = pos * uvScale;

    // Compute UV based on the chosen dominant axis
    vec2 uv;
    if (abs(bestDir.x) > abs(bestDir.y) && abs(bestDir.x) > abs(bestDir.z)) {
        uv = scaledPos.yz; // projection on YZ plane
    } else if (abs(bestDir.y) > abs(bestDir.z)) {
        uv = scaledPos.xz; // projection on XZ plane
    } else {
        uv = scaledPos.xy; // projection on XY plane
    }

    // Sample single texture
    return texture(tex, uv);
}


vec3 safeNormalize(vec3 v) {
    float len = length(v);
    return (len > 1e-6) ? v / len : vec3(0.0, 1.0, 0.0); // or some fallback
}

void main() {
    vec3 norm = safeNormalize(Normal);
    vec3 faceNorm = safeNormalize(FaceNormal);
    vec3 interpNorm = mix(norm, faceNorm, 0.5);
    float diff = clamp(0.5 + (dot(interpNorm, -lightDir)) * 0.5, 0.0, 1.0); // Clamp for safety
    
    // Model-space normal for texture orientation
    vec3 modelInterpNorm = normalize(mix(ModelNormal, ModelFaceNormal, 1.0));

    // --- Auto-UV sampling using triplanar method ---
    vec4 texColor;
    if (useSecondTexture) {
        vec4 tex1Color = quantizedTriplanarTexture(tex1, Pos, modelInterpNorm);
        vec4 tex2Color = quantizedTriplanarTexture(tex2, Pos, modelInterpNorm);
        float stripe = mod(floor(Pos.x) + floor(Pos.y) + floor(Pos.z), 2.0);
        texColor = mix(tex1Color, tex2Color, stripe);
    } else {
        texColor = quantizedTriplanarTexture(tex1, Pos, modelInterpNorm);
    }

    // --- Lighting & modulation ---
    vec4 out_col = texColor * uColor;
    out_col *= mix(minBrightness, maxBrightness, diff);
    out_col.a *= uAlphaMult;

    if (transparentWalls && faceNorm.z < 0.70710678) {
        out_col.a *= 0.5;
    }

    // --- Wireframe overlay ---
    float edge = edgeFactor();
    out_col = mix(vec4(out_col.rgb * 0.5, out_col.a), out_col, edge);

    // --- Opaque Pass ---
    if (uOpaquePass) {
        if (out_col.a < 0.999) discard;
        accum = vec4(out_col.rgb, 1.0);
        return;
    }

    // --- Transparent Pass (Weighted Blended OIT) ---
    if (out_col.a >= 0.999) discard;

    float csz = abs(1.0 / gl_FragCoord.w);
    float factor = 1.0 / 200.0;
    float z = factor * csz;
    float weight = clamp((0.03 / (1e-5 + pow(z, 4.0))), 1e-4, 3e3);

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
      out vec3 Pos; // Pass to fragment
      out vec3 ModelNormal;
      out vec3 ModelFaceNormal;

      void main() {
        Pos = aPos;
        ModelNormal = aNormal;
        ModelFaceNormal = aFaceNormal;

        FragPos = vec3(uModel * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(uModel))) * aNormal;
        FaceNormal = mat3(transpose(inverse(uModel))) * aFaceNormal;
        Bary = aBary;
        gl_Position = uProjection * uView * vec4(FragPos, 1.0);
      }

  )"""";

  }

}