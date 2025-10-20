#pragma once
#include <vector>
#include "shader.h"
#include <glm/glm.hpp>

class Mesh {
public:
  Mesh() = default;

  unsigned int VAO = 0, VBO = 0, EBO = 0, numVertices = 0;

  Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, float wireThickness, unsigned short collideMaterial);
  static Mesh createQuad(float width, float height);
  static Mesh createSphere(float radius = 1.0f, glm::vec3 offset = glm::vec3(0, 0, 0), int n_stacks = 8, int n_slices = 8);
  ~Mesh();
  void draw(Shader * shader);
  void draw();
private:
  float wireThickness;
  unsigned short collideMaterial;
};
