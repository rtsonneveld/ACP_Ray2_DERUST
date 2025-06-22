#pragma once
#include <vector>
#include "shader.h"
#include <glm/glm.hpp>

class Mesh {
public:
  Mesh() = default;

  Mesh(std::vector<float> vertices, std::vector<unsigned int> indices);
  static Mesh createSphere(float radius = 1.0f, glm::vec3 offset = glm::vec3(0, 0, 0), int n_stacks = 16, int n_slices = 16);
  ~Mesh();
  void draw(Shader * shader);
private:
  unsigned int VAO = 0, VBO = 0, EBO = 0, numVertices = 0;
};
