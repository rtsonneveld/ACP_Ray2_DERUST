#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  glm::vec3 position;

  Camera(glm::vec3 pos);
  glm::mat4 getViewMatrix();
};
