#pragma once
#include <vector>

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<float> vertices, std::vector<int> indices);
  ~Mesh();
  void draw();
private:
  unsigned int VAO = 0, VBO = 0, EBO = 0, numIndices = 0;
};
