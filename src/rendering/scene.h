#pragma once
#include <string>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "rendering/shader.h"
#include "rendering/camera.h"
#include "rendering/mesh.h"
#include "rendering/shaders/vertexShader.h"
#include "rendering/shaders/fragmentShader.h"

#include <ACP_Ray2.h>
#include <GAM/GAM.h>
#include "cpa_glm_util.h"

class Scene {
public:

  Scene();
  void init();
  void render(float display_w, float display_h);

private:

  Shader* shader;
  Camera* camera;
  Mesh* cube;

  void renderSPO(HIE_tdstSuperObject* spo);

};
