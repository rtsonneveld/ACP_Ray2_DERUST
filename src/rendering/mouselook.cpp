#include "mouselook.hpp"

MouseLook::MouseLook() {

  pitch = 0.0f;
  yaw = -90.0f;
  speed = 1.0f;
}

void MouseLook::SetFromGame(glm::vec3 pos, glm::vec3 forward) {
  yaw = glm::degrees(atan2(forward.y, forward.x));
  pitch = glm::degrees(asin(forward.z));
  position = pos;
}

void MouseLook::SetFromUser(GLFWwindow * window) {
  double xPos, yPos;
  glfwGetCursorPos(window, &xPos, &yPos);

  float xoffset = (float)xPos - (float)lastMouseX;
  float yoffset = (float)lastMouseY - (float)yPos;

  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw -= xoffset;
  pitch += yoffset;

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  float velocity = speed * (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS ? speedUpMultiplier : 1.0f);

  if (glfwGetKey(window, GLFW_KEY_W)) {
    position += forward * velocity;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    position -= forward * velocity;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    position -= glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1))) * velocity;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    position += glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1))) * velocity;
  }
}

void MouseLook::Update(GLFWwindow* window) {
  
  double xPos, yPos;
  glfwGetCursorPos(window, &xPos, &yPos);

  lastMouseX = xPos;
  lastMouseY = yPos;

  glm::vec3 direction(
    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    sin(glm::radians(yaw)) * cos(glm::radians(pitch)),
    sin(glm::radians(pitch))
  );

  forward = glm::normalize(direction);
}