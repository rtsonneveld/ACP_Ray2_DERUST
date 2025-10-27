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

uniform float wireThickness = 1.5;
uniform vec2 uScreenSize = vec2(1000.0,1000.0);

// --- Edge factor for wireframe ---
float edgeFactor() {
    vec3 d = fwidth(Bary);
    vec3 a3 = smoothstep(vec3(0.0), d * wireThickness, Bary);
    return min(min(a3.x, a3.y), a3.z);
}

// --- Generate planar UVs from FragPos and Normal ---
vec2 computeAutoUV(vec3 pos, vec3 normal) {

    vec3 n = abs(normal);
    vec2 uv;
    if (n.z >= n.x && n.z >= n.y) {
        // Surface mostly facing up/down → use X/Y
        uv = pos.xy;
    } else if (n.x >= n.y) {
        // Surface mostly facing X → use Y/Z
        uv = pos.yz;
    } else {
        // Surface mostly facing Y → use X/Z
        uv = pos.xz;
    }
    return uv;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 faceNorm = normalize(FaceNormal);
    vec3 interpNorm = mix(norm, faceNorm, 0.5);
    float diff = 0.5 + (dot(interpNorm, -lightDir)) * 0.5;
    
    // --- Auto-UVs based on model-space normals & positions ---
    vec3 modelInterpNorm = normalize(mix(ModelNormal, ModelFaceNormal, 0.5));
    vec2 autoUV = computeAutoUV(Pos, modelInterpNorm);

    // --- Sample texture(s) ---
    vec4 texColor;
    if (useSecondTexture) {
        // Alternate between tex1 and tex2 in a checker pattern
        float stripe = mod(floor(Pos.x) + floor(Pos.y) + floor(Pos.z), 2.0);
        texColor = mix(texture(tex1, autoUV), texture(tex2, autoUV), stripe);
    } else {
        texColor = texture(tex1, autoUV);
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