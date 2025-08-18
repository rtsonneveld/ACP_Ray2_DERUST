#pragma once
#include <string>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "rendering/shader.h"
#include "rendering/camera.h"
#include "rendering/mesh.h"
#include "rendering/shaders/basic.h"

#include <ACP_Ray2.h>
#include <GAM/GAM.h>
#include "cpa_glm_util.h"

class Scene {
public:

  void init();
  void render(GLFWwindow * window, float display_w, float display_h);

private:

  Shader* shader;
  Camera* camera;

  void renderSPO(HIE_tdstSuperObject* spo, bool inActiveSector);
  void renderPhysicalObject(PO_tdstPhysicalObject* po);
  void renderActorCollSet(ZDX_tdstCollSet* collSet);
  void renderZdxList(ZDX_tdstZdxList* list);
  void renderPhysicalObjectVisual(PO_tdstPhysicalObject* po);
  void renderPhysicalObjectCollision(PO_tdstPhysicalObject* po);

};
