#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
  unsigned int ID;

  Shader(const char* vertexPath, const char* fragmentPath);
  void use();
  void setColor(const std::string& name, const glm::vec4& color) const;
  void setMat4(const std::string& name, const glm::mat4& mat) const;
};
