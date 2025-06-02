#include "scene.h"

Scene::Scene() {

}

void Scene::init() {
  shader = new Shader(vertexShader, fragmentShader);
  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));
  cube = new Mesh();
}

void Scene::renderSPO(HIE_tdstSuperObject* spo) {

  auto matrix = spo->p_stGlobalMatrix;
  auto position = matrix->stPos;
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
  model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

  shader->setMat4("model", model);

  cube->draw();

  HIE_tdstSuperObject* child;
  LST_M_DynamicForEach(spo, child) {
    renderSPO(child);
  }
}

void Scene::render(float display_w, float display_h) {
  assert(shader != nullptr);
  assert(camera != nullptr);
  assert(cube != nullptr);

  shader->use();

  glm::mat4 model = glm::mat4(1.0f);
  //glm::mat4 view = camera->getViewMatrix();
  
  auto cam = GAM_g_stEngineStructure->g_hStdCamCharacter;
  auto camPos = ToGLMMat4(*cam->p_stGlobalMatrix);

  //glm::mat4 view = glm::lookAtLH(camPos, camPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0,0,1));
  glm::mat4 view = glm::inverse(camPos);
  glm::mat4 proj = glm::perspective(cam->hLinkedObject.p_stActor->hCineInfo->hCurrent->xFocal, (float)display_w / (float)display_h, 0.1f, 10000.0f);
 
  shader->setMat4("model", model);
  shader->setMat4("view", view);
  shader->setMat4("projection", proj);

  renderSPO(*GAM_g_p_stFatherSector);
  renderSPO(*GAM_g_p_stDynamicWorld);
  renderSPO(*GAM_g_p_stInactiveDynamicWorld);
}