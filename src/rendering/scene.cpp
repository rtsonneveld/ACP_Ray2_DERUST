#include "scene.hpp"
#include "geo_mesh.hpp"
#include <HIE/HIE_Const.h>
#include <LST.h>
#include "mouselook.hpp"
#include <imgui.h>
#include "mod/globals.h"
#include <ui/dialogs/inspector.hpp>
#include <ui/dialogs/options.hpp>

#include "rendering/shaders/basic.hpp"
#include "rendering/shaders/woit_fullscreenpresent.hpp"
#include "rendering/textures.hpp"

#define COLOR_ZDD glm::vec4(0.0f, 1.0f, 0.0f, 0.5f)
#define COLOR_ZDE glm::vec4(1.0f, 1.0f, 1.0f, 0.5f)
#define COLOR_ZDM glm::vec4(1.0f, 1.0f, 1.0f, 0.5f)
#define COLOR_ZDR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_DEFAULT glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

MouseLook mouseLook;
bool useMouseLook = false;
bool captureMouseLookInput = false;
bool wasTeleporting = false;

float mouseLookYaw = 0.0f, mouseLookPitch = 0.0f;

Mesh sphere;
Mesh fullScreenQuad;

GLuint oit_fbo, accum_texture, reveal_texture, rbo_depth;

void Scene::init() {

  geometryShader = new Shader(Shaders::Basic::Vertex, Shaders::Basic::Fragment);
  woitFullScreenPresentShader = new Shader(Shaders::WOIT_FullScreenPresent::Vertex, Shaders::WOIT_FullScreenPresent::Fragment);

  camera = new Camera(glm::vec3(1.5f, 1.5f, 1.5f));

  sphere = Mesh::createCube(glm::vec3(1.0f, 1.0f, 1.0f)); //Mesh::createSphere(1.0f);
  fullScreenQuad = Mesh::createQuad(1.0f, 1.0f);
}

void Scene::renderPhysicalObject(Shader* shader, PO_tdstPhysicalObject* po, bool hasNoCollisionFlag) {

  if (opt_drawCollisionZones && (!hasNoCollisionFlag || opt_drawNoCollisionObjects)) {
    renderPhysicalObjectCollision(shader, po);
  }

  if (opt_drawVisuals) {
    renderPhysicalObjectVisual(shader, po);
  }
}

void Scene::renderActorCollSet(Shader * shader, HIE_tdstSuperObject* spo, ZDX_tdstCollSet* collSet) {
  if (collSet == nullptr) return;
  if (!opt_drawCollisionZones) return;

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDD)) {
    shader->setVec4("uColor", COLOR_ZDD);
    renderZdxList(shader, collSet->hZddList, spo, ZDX_C_ucTypeZdd);
  }

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDE)) {
    shader->setVec4("uColor", COLOR_ZDE);
    renderZdxList(shader, collSet->hZdeList, spo, ZDX_C_ucTypeZde);
  }

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDM)) {
    shader->setVec4("uColor", COLOR_ZDM);
    renderZdxList(shader, collSet->hZdmList, spo, ZDX_C_ucTypeZdm);
  }

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDR)) {
    shader->setVec4("uColor", COLOR_ZDR);
    renderZdxList(shader, collSet->hZdrList, spo, ZDX_C_ucTypeZdr);
  }
}

void Scene::renderZdxList(Shader* shader, ZDX_tdstZdxList* list, HIE_tdstSuperObject* spo, unsigned char zoneType) {

  if (list == nullptr) return;

  int index = 0;

  auto hZdx = list->hGeoZdxList.hFirstElementSta;
  while (hZdx != nullptr) {

    if (hZdx->hGeoObj != nullptr && GAM_fn_hIsThisZoneActive(spo, zoneType, index)) {
      GeometricObjectMesh geoMesh = *GeometricObjectMesh::get(hZdx->hGeoObj);
      geoMesh.draw(shader);
    }

    hZdx = hZdx->hNextBrotherSta;
    index++;
  }
}

void Scene::renderPhysicalObjectVisual(Shader* shader, PO_tdstPhysicalObject* po)
{
  int nbLod = po->hVisualSet->xNbLodDefinitions;

  if (nbLod <= 0) return;
  // Only use the first LOD for now
  auto geomObj = po->hVisualSet->d_p_stLodDefinitions[0];

  GeometricObjectMesh ipoMeshVisual = *GeometricObjectMesh::get(geomObj);
  shader->setVec4("uColor", COLOR_DEFAULT);
  ipoMeshVisual.draw(shader);
}


void Scene::renderPhysicalObjectCollision(Shader* shader, PO_tdstPhysicalObject* po)
{

  auto collSet = po->hCollideSet;
  if (collSet == nullptr) return;

  if (collSet->hZdd != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDD)) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdd);
    shader->setVec4("uColor", COLOR_ZDD);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZde != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDE)) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZde);
    shader->setVec4("uColor", COLOR_ZDE);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdm != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDM)) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdm);
    shader->setVec4("uColor", COLOR_ZDM);
    ipoMeshCollision.draw(shader);
  }

  if (collSet->hZdr != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDR)) {

    if (opt_transparentZDRWalls) {
      shader->setBool("transparentWalls", TRUE);
    }

    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdr);
    shader->setVec4("uColor", COLOR_ZDR);
    ipoMeshCollision.draw(shader);

    shader->setBool("transparentWalls", FALSE);
  }
}

void Scene::renderSPO(Shader * shader, HIE_tdstSuperObject* spo, bool activeSector) {

  if (spo->ulFlags & HIE_C_Flag_Hidden && !opt_drawInvisibleObjects) return;
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

    activeSector = false;
    if (sector == currentSector) {
      activeSector = true;
    } else {

      auto iterSector = currentSector->stListOfSectorsInActivityInteraction.hFirstElementSta;
      LST_M_StaticForEach(&currentSector->stListOfSectorsInActivityInteraction, iterSector) {
        if (iterSector->hPointerOfSectorSO->hLinkedObject.p_stSector == sector) {
          activeSector = true;
        }
      }
    }
  }

  if (opt_inactiveSectorVisibility == InactiveSectorVisibility::Hidden) {
    if (!activeSector) return; 
  } else if (opt_inactiveSectorVisibility == InactiveSectorVisibility::Transparent) {
    shader->setFloat("uAlphaMult", activeSector ? 1.0f : 0.5f);
  } else if (opt_inactiveSectorVisibility == InactiveSectorVisibility::Visible) {
    shader->setFloat("uAlphaMult", 1.0f);
  }

  if (spo->ulType & HIE_C_Type_IPO) {
    auto ipo = spo->hLinkedObject.p_stInstantiatedPhysicalObject;

    if (ipo != nullptr) {
      renderPhysicalObject(shader, ipo->hPhysicalObject, spo->ulFlags & HIE_C_Flag_NotPickable);
    }
  } else if (spo->ulType & HIE_C_Type_PO) {
    auto po = spo->hLinkedObject.p_stPhysicalObject;

    if (po != nullptr) {
      renderPhysicalObject(shader, po, spo->ulFlags & HIE_C_Flag_NotPickable);
    }
  }

  if (spo->ulType & HIE_C_Type_Actor) {
    auto actor = spo->hLinkedObject.p_stActor;

    if (actor != nullptr && spo != GAM_g_stEngineStructure->g_hStdCamCharacter) {
      renderActorCollSet(shader, spo, actor->hCollSet);
    }
  }

  if (spo == g_DR_selectedObject && DR_DLG_Inspector_DebugSphereEnabled) {

    glm::vec3 position = glm::vec3(model[3]);  // last column is translation in GLM

    // Your custom scale
    glm::vec3 scale(DR_DLG_Inspector_DebugSphereRadius, DR_DLG_Inspector_DebugSphereRadius, DR_DLG_Inspector_DebugSphereRadius);

    // Build a new matrix with only position and scale
    glm::mat4 debugSphereModelMatrix = glm::translate(glm::mat4(1.0f), position);
    debugSphereModelMatrix = glm::scale(debugSphereModelMatrix, scale);

    shader->use();

    shader->setMat4("uModel", debugSphereModelMatrix);
    shader->setVec4("uColor", COLOR_DEFAULT);
    shader->setTex2D("tex1", Textures::ColDefault, 0);
    sphere.draw(shader);
  }

  HIE_tdstSuperObject* child;
  LST_M_DynamicForEach(spo, child) {
    renderSPO(shader, child, activeSector);
  }
}

int fb_width = 0;
int fb_height = 0;

void generateRenderTextures(int display_w, int display_h) {
  // Only regenerate if size changed
  if (display_w == fb_width && display_h == fb_height && oit_fbo != 0)
    return;

  fb_width = display_w;
  fb_height = display_h;

  // Delete old resources if they exist
  if (oit_fbo) {
    glDeleteFramebuffers(1, &oit_fbo);
    glDeleteTextures(1, &accum_texture);
    glDeleteTextures(1, &reveal_texture);
    glDeleteRenderbuffers(1, &rbo_depth);
  }

  // Create new framebuffer
  glGenFramebuffers(1, &oit_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);

  // Accum texture
  glGenTextures(1, &accum_texture);
  glBindTexture(GL_TEXTURE_2D, accum_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, display_w, display_h, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accum_texture, 0);

  // Reveal texture
  glGenTextures(1, &reveal_texture);
  glBindTexture(GL_TEXTURE_2D, reveal_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, display_w, display_h, 0, GL_RED, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, reveal_texture, 0);

  // Depth buffer
  glGenRenderbuffers(1, &rbo_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, display_w, display_h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

  // Draw buffers
  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  // Check framebuffer status
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer not complete!\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::renderPass(bool opaquePass, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {


  geometryShader->use();

  geometryShader->setTex2D("tex1", Textures::ColDefault, 0);
  geometryShader->setBool("useSecondTexture", FALSE);

  geometryShader->setBool("uOpaquePass", opaquePass);
  geometryShader->setMat4("uModel", model);
  geometryShader->setMat4("uView", view);
  geometryShader->setMat4("uProjection", proj);
  geometryShader->setFloat("uAlphaMult", 1.0f);

  renderSPO(geometryShader, *GAM_g_p_stFatherSector, true);
  renderSPO(geometryShader, *GAM_g_p_stDynamicWorld, true);
  renderSPO(geometryShader, *GAM_g_p_stInactiveDynamicWorld, false);
}

void Scene::render(GLFWwindow * window, float display_w, float display_h) {
  assert(geometryShader != nullptr);
  assert(camera != nullptr);

  generateRenderTextures(display_w, display_h);

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

  // Opaque pass start:
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDepthMask(GL_TRUE);
  glClearColor(0.0, 0.0, 0.0, 1.0f); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Opaque pass
  renderPass(true, model, view, proj);

  // Copy depth buffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oit_fbo);
  glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  // OIT pass
  glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);
  glViewport(0, 0, display_w, display_h);
  glDepthMask(GL_FALSE);

  static const float clear_accum[] = { 0, 0, 0, 0 };
  glClearBufferfv(GL_COLOR, 0, clear_accum);
  glBlendEquationi(0, GL_FUNC_ADD);
  glBlendFunci(0, GL_ONE, GL_ONE);

  static const float clear_reveal[] = { 1, 1, 1, 1 };
  glClearBufferfv(GL_COLOR, 1, clear_reveal);
  glBlendEquationi(1, GL_FUNC_ADD);
  glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

  // Transparent pass
  renderPass(false, model, view, proj);

  // Composite over the default buffer.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glClearColor(0, 0, 0, 1.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_TRUE);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
  glViewport(0, 0, display_w, display_h);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, accum_texture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, reveal_texture);
  
  woitFullScreenPresentShader->use();
  woitFullScreenPresentShader->setInt("accum_tex", 0);
  woitFullScreenPresentShader->setInt("reveal_tex", 1);

  fullScreenQuad.draw();

  glUseProgram(0);
}