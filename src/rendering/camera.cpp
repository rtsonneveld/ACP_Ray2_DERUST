#include "camera.h"

Camera::Camera(glm::vec3 pos) : position(pos) {}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
