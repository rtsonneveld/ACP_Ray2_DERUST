#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class MouseLook
{
  public:
    glm::vec3 position;
    glm::vec3 forward;

    void SetFromGame(glm::vec3 pos, glm::vec3 forward);
    void SetFromUser(GLFWwindow * window);
    void Update(GLFWwindow* window);
    MouseLook();
    static inline const float mouseSensitivity = 0.5f;
    static inline const float speedUpMultiplier = 4.0f;
    static inline const float minSpeed = 0.1f;
    static inline const float maxSpeed = 10.f;
  private:
    float yaw, pitch;
    float speed;
    double lastMouseX, lastMouseY;
};