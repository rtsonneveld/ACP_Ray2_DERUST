#include "shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

void checkCompileErrors(unsigned int shader, const std::string& type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
        << "\n" << infoLog << "\n" << std::endl;
    }
  }
  else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
        << "\n" << infoLog << "\n" << std::endl;
    }
  }
}


Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode) {
  
  unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");


  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() {
  glUseProgram(ID);
}

void Shader::setBool(const std::string& name, const bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, const int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUInt(const std::string& name, const unsigned int value) const {
  glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec4(const std::string& name, const glm::vec4& vec4) const {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
