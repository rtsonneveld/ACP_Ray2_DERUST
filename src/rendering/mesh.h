#pragma once

class Mesh {
public:
  Mesh();
  void draw();
private:
  unsigned int VAO, VBO, EBO;
};
