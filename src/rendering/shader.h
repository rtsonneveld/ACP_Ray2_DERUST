#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
  unsigned int ID;

  Shader(const char* vertexPath, const char* fragmentPath);
  void use();
  void setMat4(const std::string& name, const glm::mat4& mat) const;
};
