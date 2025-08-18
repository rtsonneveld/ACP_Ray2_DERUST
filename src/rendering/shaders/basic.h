#pragma once

namespace Shaders {

  namespace Basic {

    inline const char* Fragment = R""""(    

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
    inline const char* Vertex = R""""(

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
  }

}