#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
  unsigned int ID;

  Shader(const char* vertexPath, const char* fragmentPath);
  void use();
  void setFloat(const std::string& name, const float value) const;
  void setVec4(const std::string& name, const glm::vec4& vec4) const;
  void setMat4(const std::string& name, const glm::mat4& mat) const;
};
