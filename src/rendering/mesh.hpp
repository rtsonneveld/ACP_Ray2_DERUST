#pragma once
#include <vector>
#include "shader.hpp"
#include <glm/glm.hpp>

class Mesh {
public:
  Mesh() = default;

  unsigned int VAO = 0, VBO = 0, EBO = 0, numVertices = 0;

  Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, float wireThickness);
  static Mesh createQuad(float width, float height);
  static Mesh createCube(glm::vec3 size, glm::vec3 offset = glm::vec3(0, 0, 0));
  static Mesh createOctahedron(glm::vec3 size, glm::vec3 offset = glm::vec3(0, 0, 0));
  static Mesh createSphere(float radius = 1.0f, glm::vec3 offset = glm::vec3(0, 0, 0), int n_stacks = 16, int n_slices = 16);
  ~Mesh();
  void draw(Shader * shader);
  void draw();
private:
  float wireThickness;
};
