#pragma once
#include "glm/glm.hpp"
class Shader;

namespace RenderUtil {

  void Init();
  void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color = { 1.0, 1.0, 1.0, 1.0 });
  void DrawArrow(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color = { 1.0, 1.0, 1.0, 1.0 });
  void DrawTriangle(Shader* shader, glm::vec3 A, glm::vec3 B, glm::vec3 C, unsigned int texture, glm::vec4 color);
}