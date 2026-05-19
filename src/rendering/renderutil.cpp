#include "shader.hpp"
#include "mesh.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "renderutil.hpp"

namespace RenderUtil {

  static Mesh lineBox;
  static Mesh lineCone;
  static Mesh triangle;

  void Init() {
    lineBox = Mesh::createCube(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
    lineCone = Mesh::createCone(1.0f, 1.0f, 3, glm::vec3(0, 0, 0));
    triangle = Mesh({
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
    }, {
      0, 1, 2
    }, 0.0);
  }

  void DrawMeshInDirection(Mesh* mesh, Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color)
  {
    if (glm::distance(A, B) < 0.001f) {
      return;
    }
    glm::vec3 dir = glm::normalize(B - A);
    float lineLength = glm::length(B - A);

    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), -dir, glm::vec3(0, 1, 0)));

    glm::mat4 lineMat = glm::mat4(1.0f);
    lineMat = glm::translate(lineMat, A);
    lineMat = lineMat * rotation;

    lineMat = glm::translate(lineMat, glm::vec3(0.0f, 0.0f, lineLength * 0.5f));
    lineMat = glm::scale(lineMat, glm::vec3(thickness, thickness, lineLength));

    shader->setMat4("uModel", lineMat);
    shader->setTex2D("tex1", texture, 0);
    shader->setTex2D("tex2", texture, 0);
    shader->setVec4("uColor", color);
    mesh->draw();
    shader->setVec4("uColor", glm::vec4(1, 1, 1, 1));
  }

  void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color)
  {
    DrawMeshInDirection(&lineBox, shader, A, B, texture, thickness, color);
  }

  void DrawArrow(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness, glm::vec4 color)
  {

    float dist = glm::distance(A, B);
    if (dist < 0.001f) return;

    // 2. Calculate Arrow Head
    glm::vec3 dir = glm::normalize(B - A);

    float headLength = dist * 0.2f; // Head is 20% of shaft length
    if (headLength > thickness * 10.0f) {
      headLength = thickness * 10.0f;
    }
    DrawMeshInDirection(&lineCone, shader, A + (dist - headLength) * dir, A, texture, thickness, color);
    DrawMeshInDirection(&lineCone, shader, A + (dist - headLength) * dir, B, texture, thickness * 3.0f, color);
  }

  void DrawTriangle(Shader* shader, glm::vec3 A, glm::vec3 B, glm::vec3 C, unsigned int texture, glm::vec4 color) {

    glm::vec3 col0 = B - A;                 // Maps the X-axis segment
    glm::vec3 col1 = C - B;                 // Maps the Y-axis segment
    glm::vec3 col2 = glm::cross(col0, col1); // Orthogonal Z-axis vector
    glm::vec3 col3 = A;                     // Translation (where 0,0,0 goes)

    glm::mat4 transform(1.0f);
    transform[0] = glm::vec4(col0, 0.0f);
    transform[1] = glm::vec4(col1, 0.0f);
    transform[2] = glm::vec4(col2, 0.0f);
    transform[3] = glm::vec4(col3, 1.0f);

    shader->setMat4("uModel", transform);
    shader->setTex2D("tex1", texture, 0);
    shader->setTex2D("tex2", texture, 0);
    shader->setVec4("uColor", color);
    triangle.draw();
  }
}