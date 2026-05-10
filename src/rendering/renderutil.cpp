#include "shader.hpp"
#include "mesh.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "renderutil.hpp"

namespace RenderUtil {

  static Mesh lineBox;

  void Init() {
    lineBox = Mesh::createCube(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
  }

  void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color)
  {
    if (glm::distance(A, B) < 0.001f) {
      return;
    }
    glm::vec3 dir = glm::normalize(B - A);
    float lineLength = glm::length(B - A);

    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0, 1, 0)));

    glm::mat4 lineMat = glm::mat4(1.0f);
    lineMat = glm::translate(lineMat, A);
    lineMat = lineMat * rotation;

    lineMat = glm::translate(lineMat, glm::vec3(0.0f, 0.0f, -lineLength * 0.5f));
    lineMat = glm::scale(lineMat, glm::vec3(thickness, thickness, lineLength));

    shader->setMat4("uModel", lineMat);
    shader->setTex2D("tex1", texture, 0);
    shader->setTex2D("tex2", texture, 0);
    shader->setVec4("uColor", color);
    lineBox.draw();
    shader->setVec4("uColor", glm::vec4(1,1,1,1));
  }

  void DrawArrow(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color)
  {
    // 1. Draw the main shaft
    DrawLine(shader, A, B, texture, thickness, color);

    float dist = glm::distance(A, B);
    if (dist < 0.001f) return;

    // 2. Calculate Arrow Head
    glm::vec3 dir = glm::normalize(B - A);

    // Determine an orthogonal vector for the "wings" of the arrow head
    glm::vec3 up = (glm::abs(dir.y) > 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(dir, up));
    glm::vec3 headUp = glm::cross(right, dir);

    float headLength = dist * 0.2f; // Head is 20% of shaft length
    float headWidth = thickness * 3.0f; // Head is wider than the shaft

    // Points for the arrow "wings"
    glm::vec3 leftWing = B - (dir * headLength) + (right * headWidth);
    glm::vec3 rightWing = B - (dir * headLength) - (right * headWidth);

    // 3. Draw the head pieces
    DrawLine(shader, B, leftWing, texture, thickness, color);
    DrawLine(shader, B, rightWing, texture, thickness, color);
  }

}