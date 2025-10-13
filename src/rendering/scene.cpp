#include "scene.h"
#include "geo_mesh.h"
#include <HIE/HIE_Const.h>
#include <LST.h>
#include "mouselook.h"
#include <imgui.h>
#include "mod/globals.h"
#include <ui/dialogs/inspector.hpp>

#define COLOR_ZDD glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)
#define COLOR_ZDE glm::vec4(1.0f, 1.0f, 0.0f, 0.5f)
#define COLOR_ZDM glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)
#define COLOR_ZDR glm::vec4(0.75f, 0.75f, 1.0f, 1.0f)
#define COLOR_DEFAULT glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

extern bool dbg_drawCollision;
extern bool dbg_drawVisuals;
extern bool dbg_drawZDD;
extern bool dbg_drawZDE;
extern bool dbg_drawZDM;
extern bool dbg_drawZDR;
extern bool dbg_transparentZDRWalls;

MouseLook mouseLook;
bool useMouseLook = false;
bool captureMouseLookInput = false;
bool wasTeleporting = false;

float mouseLookYaw = 0.0f, mouseLookPitch = 0.0f;

Mesh sphere;

void Scene::init() {
  shader = new Shader(Shaders::Basic::Vertex, Shaders::Basic::Fragment);
  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));

  sphere = Mesh::createSphere(1.0f);
}

void Scene::renderPhysicalObject(PO_tdstPhysicalObject* po, bool hasNoCollisionFlag) {
  if (dbg_drawCollision && !hasNoCollisionFlag) {
    renderPhysicalObjectCollision(po);
  }

  if (dbg_drawVisuals) {
    renderPhysicalObjectVisual(po);
  }
}

void Scene::renderActorCollSet(ZDX_tdstCollSet* collSet) {
  if (collSet == nullptr) return;
  if (!dbg_drawCollision) return;

  if (dbg_drawZDD) {
    shader->setVec4("uColor", COLOR_ZDD);
    renderZdxList(collSet->hZddList);
  }

  if (dbg_drawZDE) {
    shader->setVec4("uColor", COLOR_ZDE);
    renderZdxList(collSet->hZdeList);
  }

  if (dbg_drawZDM) {
    shader->setVec4("uColor", COLOR_ZDM);
    renderZdxList(collSet->hZdmList);
  }

  if (dbg_drawZDR) {
    shader->setVec4("uColor", COLOR_ZDR);
    renderZdxList(collSet->hZdrList);
  }
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
  shader->setVec4("uColor", COLOR_DEFAULT);
  ipoMeshVisual.draw(shader);
}


void Scene::renderPhysicalObjectCollision(PO_tdstPhysicalObject* po)
{
  auto collSet = po->hCollideSet;
  if (collSet == nullptr) return;

  if (collSet->hZdd != nullptr && dbg_drawZDD) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdd);
    shader->setVec4("uColor", COLOR_ZDD);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZde != nullptr && dbg_drawZDE) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZde);
    shader->setVec4("uColor", COLOR_ZDE);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdm != nullptr && dbg_drawZDM) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdm);
    shader->setVec4("uColor", COLOR_ZDM);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdr != nullptr && dbg_drawZDR) {

    if (dbg_transparentZDRWalls) {
      shader->setBool("transparentWalls", TRUE);
    }

    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdr);
    shader->setVec4("uColor", COLOR_ZDR);
    ipoMeshCollision.draw(shader);

    shader->setBool("transparentWalls", FALSE);
  }
}

void Scene::renderSPO(HIE_tdstSuperObject* spo, bool inActiveSector) {

  if (spo->ulFlags & HIE_C_Flag_Hidden) return;
  glm::mat4 model = ToGLMMat4(*spo->p_stGlobalMatrix);

  shader->setMat4("uModel", model);

  bool mirrored = glm::determinant(model) < 0.0f;

  if (mirrored) {
    glFrontFace(GL_CW); // Clockwise is front-facing
  }  else {
    glFrontFace(GL_CCW); // Default: Counter-clockwise is front-facing
  }

  if (spo->ulType & HIE_C_Type_Sector) {

    auto sector = spo->hLinkedObject.p_stSector;
    auto currentSector = GAM_g_stEngineStructure->g_hMainActor->hLinkedObject.p_stActor->hSectInfo->hCurrentSector->hLinkedObject.p_stSector;

    inActiveSector = false;
    if (sector == currentSector) {
      inActiveSector = true;
    } else {
      
      auto iterSector = currentSector->stListOfSectorsInActivityInteraction.hFirstElementSta;
      LST_M_StaticForEach(&currentSector->stListOfSectorsInActivityInteraction, iterSector) {
        if (iterSector->hPointerOfSectorSO->hLinkedObject.p_stSector == sector) {
          inActiveSector = true;
        }
      }
    }
  }

  shader->setFloat("uAlphaMult", inActiveSector ? 1.0f : 0.5f);

  if (spo->ulType & HIE_C_Type_IPO) {
    auto ipo = spo->hLinkedObject.p_stInstantiatedPhysicalObject;

    if (ipo != nullptr) {
      renderPhysicalObject(ipo->hPhysicalObject, spo->ulFlags & HIE_C_Flag_NotPickable);
    }
  } else if (spo->ulType & HIE_C_Type_PO) {
    auto po = spo->hLinkedObject.p_stPhysicalObject;

    if (po != nullptr) {
      renderPhysicalObject(po, spo->ulFlags & HIE_C_Flag_NotPickable);
    }
  }

  if (spo->ulType & HIE_C_Type_Actor) {
    auto actor = spo->hLinkedObject.p_stActor;

    if (actor != nullptr && spo != GAM_g_stEngineStructure->g_hStdCamCharacter) {
      renderActorCollSet(actor->hCollSet);
    }
  }

  if (spo == g_DR_selectedObject && DR_DLG_Inspector_DebugSphereEnabled) {

    glm::vec3 position = glm::vec3(model[3]);  // last column is translation in GLM

    // Your custom scale
    glm::vec3 scale(DR_DLG_Inspector_DebugSphereRadius, DR_DLG_Inspector_DebugSphereRadius, DR_DLG_Inspector_DebugSphereRadius);

    // Build a new matrix with only position and scale
    glm::mat4 debugSphereModelMatrix = glm::translate(glm::mat4(1.0f), position);
    debugSphereModelMatrix = glm::scale(debugSphereModelMatrix, scale);
    shader->setMat4("uModel", debugSphereModelMatrix);
    shader->setVec4("uColor", COLOR_DEFAULT);
    sphere.draw(shader);
  }

  HIE_tdstSuperObject* child;
  LST_M_DynamicForEach(spo, child) {
    renderSPO(child, inActiveSector);
  }
}

void Scene::render(GLFWwindow * window, float display_w, float display_h) {
  assert(shader != nullptr);
  assert(camera != nullptr);

  glViewport(0, 0, display_w, display_h);
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
        g_DR_rayman->hFatherDyn = *GAM_g_p_stDynamicWorld;
        g_DR_rayman->p_stLocalMatrix->stPos = newPos;
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
  shader->setFloat("uAlphaMult", 1.0f);
  shader->use();

  renderSPO(*GAM_g_p_stFatherSector, true);
  renderSPO(*GAM_g_p_stDynamicWorld, true);
  renderSPO(*GAM_g_p_stInactiveDynamicWorld, false);
}