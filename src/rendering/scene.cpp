#include "scene.h"
#include "geo_mesh.h"
#include <HIE/HIE_Const.h>
#include <GLFW/glfw3.h>
#include <LST.h>

extern bool dbg_drawCollision;
extern bool dbg_drawVisuals;

Scene::Scene() {

}

void Scene::init() {
  shader = new Shader(vertexShader, fragmentShader);
  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));
  cube = new Mesh();
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
      geoMesh.draw();
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
  ipoMeshVisual.draw();
}


void Scene::renderPhysicalObjectCollision(PO_tdstPhysicalObject* po)
{
  auto collSet = po->hCollideSet;
  if (collSet == nullptr) return;

  if (collSet->hZdd != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdd);
    ipoMeshCollision.draw();
  }

  if (collSet->hZde != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZde);
    ipoMeshCollision.draw();
  }

  if (collSet->hZdm != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdm);
    ipoMeshCollision.draw();
  }

  if (collSet->hZdr != nullptr) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdr);
    ipoMeshCollision.draw();
  }
}

void Scene::renderSPO(HIE_tdstSuperObject* spo) {

  glm::mat4 model = ToGLMMat4(*spo->p_stGlobalMatrix);

  shader->setMat4("model", model);

  cube->draw();
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