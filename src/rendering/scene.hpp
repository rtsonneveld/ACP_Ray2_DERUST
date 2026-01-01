#pragma once
#include <string>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "rendering/shader.hpp"
#include "rendering/camera.hpp"
#include "rendering/mesh.hpp"

#include <ACP_Ray2.h>
#include <GAM/GAM.h>
#include "cpa_glm_util.hpp"

class Scene {
public:

  void init();
  GLuint getColorTexture() const;
  void render(GLFWwindow * window, float display_w, float display_h);
  void setCameraPosition(glm::vec3 from, glm::vec3 to);

private:

  Shader* geometryShader;
  Shader* woitFullScreenPresentShader;
  Shader* woitTransparencyShader;

  void renderPass(bool isTransparent, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
  void renderSPO(Shader * shader, HIE_tdstSuperObject* spo, bool inActiveSector, const glm::mat4 view, const glm::mat4 proj, glm::vec3 mainCharPos);
  void renderPhysicalObject(Shader* shader, PO_tdstPhysicalObject* po, bool hasNoCollisionFlag);
  void renderActorCollSet(Shader * shader, HIE_tdstSuperObject* spo, ZDX_tdstCollSet* collSet);
  void renderZdxList(Shader* shader, ZDX_tdstZdxList* list, HIE_tdstSuperObject* spo, unsigned char zoneType);
  void renderPhysicalObjectVisual(Shader * shader, PO_tdstPhysicalObject* po);
  void renderPhysicalObjectCollision(Shader* shader, PO_tdstPhysicalObject* po);

};