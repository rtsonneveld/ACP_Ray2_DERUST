#include "scene.h"
#include "po_mesh.h"
#include <HIE/HIE_Const.h>
#include <GLFW/glfw3.h>

Scene::Scene() {

}

void Scene::init() {
  shader = new Shader(vertexShader, fragmentShader);
  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));
  cube = new Mesh();
}

void Scene::renderSPO(HIE_tdstSuperObject* spo) {

  glm::mat4 model = ToGLMMat4(*spo->p_stGlobalMatrix);
  //model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

  shader->setMat4("model", model);

  cube->draw();
  if (spo->ulType & HIE_C_Type_IPO) {
    auto ipo = spo->hLinkedObject.p_stInstantiatedPhysicalObject;

    if (ipo != nullptr) {
      PhysicalObjectMesh ipoMesh = *PhysicalObjectMesh::get(ipo->hPhysicalObject);
      ipoMesh.draw();
    }
  } else if (spo->ulType & HIE_C_Type_PO) {
    auto po = spo->hLinkedObject.p_stPhysicalObject;

    if (po != nullptr) {
      PhysicalObjectMesh poMesh = *PhysicalObjectMesh::get(po);
      poMesh.draw();
    }
  }

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
  
  auto cam = GAM_g_stEngineStructure->g_hStdCamCharacter;
  auto camMatrix = ToGLMMat4(*cam->p_stLocalMatrix);

  //glm::mat4 view = glm::inverse(camMatrix);
  
  float time = glfwGetTime();
  float camX = cos(time) * 10.0f;
  float camY = sin(time) * 10.0f;

  auto camVec = ToGLMVec(cam->p_stGlobalMatrix->stPos);
  
  glm::vec3 forward = -glm::vec3(camMatrix[1]); // Extract and negate the Z-axis

  glm::mat4 view = glm::lookAtRH(camVec, camVec + forward, glm::vec3(camMatrix[2]));
  
  glm::mat4 proj = glm::perspective(cam->hLinkedObject.p_stActor->hCineInfo->hCurrent->xFocal, (float)display_w / (float)display_h, 0.1f, 10000.0f);
 
  shader->setMat4("model", model);
  shader->setMat4("view", view);
  shader->setMat4("projection", proj);

  renderSPO(*GAM_g_p_stFatherSector);
  renderSPO(*GAM_g_p_stDynamicWorld);
  renderSPO(*GAM_g_p_stInactiveDynamicWorld);
}