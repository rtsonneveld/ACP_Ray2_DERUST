#include "scene.h"
#include "geo_mesh.h"
#include <HIE/HIE_Const.h>
#include <LST.h>
#include "mouselook.h"
#include <imgui.h>
#include "mod/globals.h"

extern bool dbg_drawCollision;
extern bool dbg_drawVisuals;

MouseLook mouseLook;
bool useMouseLook = false;
bool captureMouseLookInput = false;
bool wasTeleporting = false;

float mouseLookYaw = 0.0f, mouseLookPitch = 0.0f;

void Scene::init() {
  shader = new Shader(vertexShader, fragmentShader);
  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));
}

void Scene::renderPhysicalObject(PO_tdstPhysicalObject* po) {
  if (dbg_drawCollision) {
    renderPhysicalObjectCollision(po);
  }

  if (dbg_drawVisuals) {
    renderPhysicalObjectVisual(po);
  }
}

void Scene::renderActorCollSet(ZDX_tdstCollSet* collSet) {
  if (collSet == nullptr) return;
  if (!dbg_drawCollision) return;

  renderZdxList(collSet->hZddList);
  renderZdxList(collSet->hZdeList);
  renderZdxList(collSet->hZdmList);
  renderZdxList(collSet->hZdrList);
}

void Scene::renderZdxList(ZDX_tdstZdxList* list) {

  if (list == nullptr) return;

  auto hZdx = list->hGeoZdxList.hFirstElementSta;
  while (hZdx != nullptr) {

    if (hZdx->hGeoObj != nullptr) {
      GeometricObjectMesh geoMesh = *GeometricObjectMesh::get(hZdx->hGeoObj);
      geoMesh.draw(shader);
    }

    hZdx = hZdx->hNextBrotherSta;
  }
}

void Scene::renderPhysicalObjectVisual(PO_tdstPhysicalObject* po)
{
  int nbLod = po->hVisualSet->xNbLodDefinitions;

  if (nbLod <= 0) return;
  // Only use the first LOD for now
  auto geomObj = po->hVisualSet->d_p_stLodDefinitions[0];

  GeometricObjectMesh ipoMeshVisual = *GeometricObjectMesh::get(geomObj);
  ipoMeshVisual.draw(shader);
}


void Scene::renderPhysicalObjectCollision(PO_tdstPhysicalObject* po)
{
  auto collSet = po->hCollideSet;
  if (collSet == nullptr) return;

  if (collSet->hZdd != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdd);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZde != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZde);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdm != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdm);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdr != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdr);
    ipoMeshCollision.draw(shader);
  }
}

void Scene::renderSPO(HIE_tdstSuperObject* spo) {

  glm::mat4 model = ToGLMMat4(*spo->p_stGlobalMatrix);

  shader->setMat4("uModel", model);

  if (spo->ulType & HIE_C_Type_IPO) {
    auto ipo = spo->hLinkedObject.p_stInstantiatedPhysicalObject;

    if (ipo != nullptr) {
      renderPhysicalObject(ipo->hPhysicalObject);
    }
  } else if (spo->ulType & HIE_C_Type_PO) {
    auto po = spo->hLinkedObject.p_stPhysicalObject;

    if (po != nullptr) {
      renderPhysicalObject(po);
    }
  }

  if (spo->ulType & HIE_C_Type_Actor) {
    auto actor = spo->hLinkedObject.p_stActor;

    if (actor != nullptr && spo != GAM_g_stEngineStructure->g_hStdCamCharacter) {
      renderActorCollSet(actor->hCollSet);
    }
  }

  HIE_tdstSuperObject* child;
  LST_M_DynamicForEach(spo, child) {
    renderSPO(child);
  }
}

void Scene::render(GLFWwindow * window, float display_w, float display_h) {
  assert(shader != nullptr);
  assert(camera != nullptr);

  glm::mat4 model = glm::mat4(1.0f);
  
  auto cam = GAM_g_stEngineStructure->g_hStdCamCharacter;
  auto camMatrix = ToGLMMat4(*cam->p_stLocalMatrix);

  captureMouseLookInput = ImGui::IsMouseDown(ImGuiMouseButton_Right);
  if (captureMouseLookInput) {
    useMouseLook = true;
  }
  if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      useMouseLook = false;
  }

  glm::mat4 view;
  if (useMouseLook) {
    if (captureMouseLookInput) {
      mouseLook.SetFromUser(window);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      // Teleport with space
      if (ImGui::IsKeyDown(ImGuiKey_Space)) {
        auto newPos = FromGLMVec(mouseLook.position);
        g_DR_rayman->p_stGlobalMatrix->stPos = newPos;
        auto dynam = g_DR_rayman->hLinkedObject.p_stActor->hDynam->p_stDynamics;
        dynam->stDynamicsBase.stPreviousMatrix.stPos = newPos;
        dynam->stDynamicsBase.stCurrentMatrix.stPos = newPos;
        *GAM_g_ucIsEdInGhostMode = true;
        wasTeleporting = true;
      }
    }
    view = glm::lookAtRH(mouseLook.position, mouseLook.position + mouseLook.forward, glm::vec3(0,0,1));
  } else {
    mouseLook.SetFromGame(ToGLMVec(cam->p_stGlobalMatrix->stPos), -glm::vec3(camMatrix[1]));
    view = glm::lookAtRH(mouseLook.position, mouseLook.position + mouseLook.forward, glm::vec3(camMatrix[2]));
  }

  if (!useMouseLook || !captureMouseLookInput) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (wasTeleporting) {
      wasTeleporting = false;
      *GAM_g_ucIsEdInGhostMode = false;
    }
  }

  mouseLook.Update(window);

  glm::mat4 proj = glm::perspective(1.0f / cam->hLinkedObject.p_stActor->hCineInfo->hCurrent->xFocal, (float)display_w / (float)display_h, 0.1f, 10000.0f);
 
  shader->setMat4("uModel", model);
  shader->setMat4("uView", view);
  shader->setMat4("uProjection", proj);
  shader->use();

  renderSPO(*GAM_g_p_stFatherSector);
  renderSPO(*GAM_g_p_stDynamicWorld);
  renderSPO(*GAM_g_p_stInactiveDynamicWorld);
}