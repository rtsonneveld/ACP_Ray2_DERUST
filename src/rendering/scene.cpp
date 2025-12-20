#include "scene.hpp"
#include "geo_mesh.hpp"
#include <HIE/HIE_Const.h>
#include <LST.h>
#include "mouselook.hpp"
#include <imgui.h>
#include "mod/globals.h"
#include "mod/util.h"
#include "mod/cheats.h"
#include <ui/dialogs/inspector.hpp>
#include <ui/dialogs/options.hpp>
#include <ui/dialogs/utils.hpp>
#include <ui/settings.hpp>

#include "rendering/shaders/basic.hpp"
#include "rendering/shaders/woit_fullscreenpresent.hpp"
#include "rendering/textures.hpp"


#define COLOR_ZDD glm::vec4(1.0f, 1.0f, 1.0f, 0.5f)
#define COLOR_ZDE glm::vec4(1.0f, 1.0f, 1.0f, 0.8f)
#define COLOR_ZDM glm::vec4(1.0f, 1.0f, 1.0f, 0.8f)
#define COLOR_ZDR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_DEFAULT glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

MouseLook mouseLook;
bool useMouseLook = false;
bool captureMouseLookInput = false;
bool wasTeleporting = false;

float mouseLookYaw = 0.0f, mouseLookPitch = 0.0f;

Mesh sphere;
Mesh tooFarLimitMesh;
Mesh fullScreenQuad;
Mesh sectorBordersCube;

GLuint oit_fbo, accum_texture, reveal_texture, rbo_depth;

void Scene::init() {

  geometryShader = new Shader(Shaders::Basic::Vertex, Shaders::Basic::Fragment);
  woitFullScreenPresentShader = new Shader(Shaders::WOIT_FullScreenPresent::Vertex, Shaders::WOIT_FullScreenPresent::Fragment);

  sphere = Mesh::createSphere(1.0f);
  tooFarLimitMesh = Mesh::createOctahedron(glm::vec3(1, 1, 1));
  fullScreenQuad = Mesh::createQuad(1.0f, 1.0f);
  sectorBordersCube = Mesh::createCube(glm::vec3(1.0f, 1.0f, 1.0f));
}

void Scene::renderPhysicalObject(Shader* shader, PO_tdstPhysicalObject* po, bool hasNoCollisionFlag) {

  if (g_DR_settings.opt_drawCollisionZones && (!hasNoCollisionFlag || g_DR_settings.opt_drawNoCollisionObjects)) {
    renderPhysicalObjectCollision(shader, po);
  }

  if (g_DR_settings.opt_drawVisuals) {
    renderPhysicalObjectVisual(shader, po);
  }
}

void Scene::renderActorCollSet(Shader * shader, HIE_tdstSuperObject* spo, ZDX_tdstCollSet* collSet) {
  if (collSet == nullptr) return;
  if (!g_DR_settings.opt_drawCollisionZones) return;

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDD)) {
    shader->setVec4("uColor", COLOR_ZDD);
    renderZdxList(shader, collSet->hZddList, spo, ZDX_C_ucTypeZdd);
  }

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDE)) {
    shader->setVec4("uColor", COLOR_ZDE);
    shader->setVec3("uvScale", glm::vec3(2));
    renderZdxList(shader, collSet->hZdeList, spo, ZDX_C_ucTypeZde);
    shader->setVec3("uvScale", glm::vec3(1));
  }

  if (IsCollisionZoneEnabled(CollisionZoneMask::ZDM)) {
    shader->setVec4("uColor", COLOR_ZDM);
    shader->setVec3("uvScale", glm::vec3(2));
    renderZdxList(shader, collSet->hZdmList, spo, ZDX_C_ucTypeZdm);
    shader->setVec3("uvScale", glm::vec3(1));
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

    if (hZdx->hGeoObj != nullptr) {

      bool isActive = GAM_fn_hIsThisZoneActive(spo, zoneType, index);
      if (isActive || g_DR_settings.opt_inactiveZoneVisibility != InactiveItemVisibility::Hidden) {

        shader->setFloat("uAlphaMult", 1.0f);

        if (!isActive && g_DR_settings.opt_inactiveZoneVisibility == InactiveItemVisibility::Transparent) {
            shader->setFloat("uAlphaMult", 0.25f);
        }
        GeometricObjectMesh geoMesh = *GeometricObjectMesh::get(hZdx->hGeoObj);
        geoMesh.draw(shader, zoneType);
      }
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
    ipoMeshCollision.draw(shader, ZDX_C_ucTypeZdd);
  }

  if (collSet->hZde != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDE)) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZde);
    shader->setVec4("uColor", COLOR_ZDE);
    ipoMeshCollision.draw(shader, ZDX_C_ucTypeZde);
  }

  if (collSet->hZdm != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDM)) {
    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdm);
    shader->setVec4("uColor", COLOR_ZDM);
    ipoMeshCollision.draw(shader, ZDX_C_ucTypeZdm);
  }

  if (collSet->hZdr != nullptr && IsCollisionZoneEnabled(CollisionZoneMask::ZDR)) {

    if (g_DR_settings.opt_transparentZDRSlopes) {
      shader->setFloat("transparentSlopesAlpha", g_DR_settings.opt_transparentZDRSlopesAlpha);
      shader->setFloat("transparentSlopesMin", g_DR_settings.opt_transparentZDRSlopesMin);
      shader->setFloat("transparentSlopesMax", g_DR_settings.opt_transparentZDRSlopesMax);
      shader->setBool("transparentSlopesInvert", g_DR_settings.opt_transparentZDRSlopesInvert);
    }

    GeometricObjectMesh ipoMeshCollision = *GeometricObjectMesh::get(collSet->hZdr);
    shader->setVec4("uColor", COLOR_ZDR);

    glEnable(GL_CULL_FACE); // For ZDR geometry we'll enable backface culling
    ipoMeshCollision.draw(shader, ZDX_C_ucTypeZdr);
    glDisable(GL_CULL_FACE);

    shader->setFloat("transparentSlopesAlpha", 1.0f);
    shader->setBool("transparentSlopesInvert", false);
  }
}

// Build rows explicitly from column-major glm::mat4
static inline glm::vec4 row(const glm::mat4& m, int r) {
  return glm::vec4(m[0][r], m[1][r], m[2][r], m[3][r]);
}

// Returns true if AABB (minPoint, maxPoint) is at least partly inside frustum
bool isSectorInFrustum(const glm::vec3& minPoint, const glm::vec3& maxPoint,
  const glm::mat4& view, const glm::mat4& proj)
{
  glm::mat4 clip = proj * view;

  // build rows
  glm::vec4 r0 = row(clip, 0);
  glm::vec4 r1 = row(clip, 1);
  glm::vec4 r2 = row(clip, 2);
  glm::vec4 r3 = row(clip, 3);

  glm::vec4 planes[6];
  // Note: rows are used — this avoids mixing axes with glm column storage
  planes[0] = r3 + r0; // left
  planes[1] = r3 - r0; // right
  planes[2] = r3 + r1; // bottom
  planes[3] = r3 - r1; // top
  planes[4] = r3 + r2; // near
  planes[5] = r3 - r2; // far

  // normalize planes
  for (int i = 0; i < 6; ++i) {
    float len = glm::length(glm::vec3(planes[i]));
    if (len > 0.0f) planes[i] /= len;
  }

  // test AABB using "positive vertex" method
  for (int i = 0; i < 6; ++i) {
    glm::vec4 p = planes[i];
    glm::vec3 normal(p.x, p.y, p.z);

    // choose the vertex most likely to be outside the plane
    glm::vec3 positive = minPoint;
    if (normal.x >= 0.0f) positive.x = maxPoint.x;
    if (normal.y >= 0.0f) positive.y = maxPoint.y;
    if (normal.z >= 0.0f) positive.z = maxPoint.z;

    float distance = glm::dot(normal, positive) + p.w;
    if (distance < 0.0f) {
      // completely outside this plane
      return false;
    }
  }

  return true;
}

void Scene::renderSPO(Shader * shader, HIE_tdstSuperObject* spo, bool activeSector, const glm::mat4 view, const glm::mat4 proj, const glm::vec3 mainCharPos) {

  if (spo->ulFlags & HIE_C_Flag_Hidden && !g_DR_settings.opt_drawInvisibleObjects) return;
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

    if (sector->bVirtual && !g_DR_settings.opt_drawVirtualSectors) return;

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

    const glm::vec3 minPoint = ToGLMVec(sector->a_stMinMaxPoints[0]);
    const glm::vec3 maxPoint = ToGLMVec(sector->a_stMinMaxPoints[1]);

    if (!isSectorInFrustum(minPoint, maxPoint, view, proj)) {
      return;
    }
  }

  if (g_DR_settings.opt_inactiveSectorVisibility == InactiveItemVisibility::Hidden) {
    if (!activeSector) return; 
  } else if (g_DR_settings.opt_inactiveSectorVisibility == InactiveItemVisibility::Transparent) {
    shader->setFloat("uAlphaMult", activeSector ? 1.0f : 0.5f);
  } else if (g_DR_settings.opt_inactiveSectorVisibility == InactiveItemVisibility::Visible) {
    shader->setFloat("uAlphaMult", 1.0f);
  }


  if (spo->ulType & HIE_C_Type_Sector && g_DR_settings.opt_drawSectorBorders) {

    auto sector = spo->hLinkedObject.p_stSector;

    auto minPoint = sector->a_stMinMaxPoints[0];
    auto maxPoint = sector->a_stMinMaxPoints[1];

    glm::vec3 scale(
      maxPoint.x - minPoint.x,
      maxPoint.y - minPoint.y,
      maxPoint.z - minPoint.z
    );

    glm::vec3 pos(
      minPoint.x + (maxPoint.x - minPoint.x) * 0.5f,
      minPoint.y + (maxPoint.y - minPoint.y) * 0.5f,
      minPoint.z + (maxPoint.z - minPoint.z) * 0.5f
    );

    glm::mat4 sectorMatrix = glm::translate(glm::mat4(1.0f), pos);
    sectorMatrix = glm::scale(sectorMatrix, scale);

    shader->use();
    shader->setMat4("uModel", sectorMatrix);
    shader->setTex2D("tex1", Textures::Sectorborder, 0);
    shader->setVec3("uvScale", scale);
    shader->setBool("useSecondTexture", false);
    sectorBordersCube.draw(shader);

    shader->setVec3("uvScale", glm::vec3(1, 1, 1));
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

    if (actor->hStandardGame != nullptr && actor->hStandardGame->ucTooFarLimit > 0) {

      float limit = actor->hStandardGame->ucTooFarLimit;

      if (limit >= g_DR_settings.opt_tooFarLimitMinSize && limit <= g_DR_settings.opt_tooFarLimitMaxSize) {

        if (actor->hStandardGame->ulCustomBits & Std_C_CustBit_NoAnimPlayerWhenTooFar && g_DR_settings.opt_showTooFarLimitAnim ||
          actor->hStandardGame->ulCustomBits & Std_C_CustBit_NoMecaWhenTooFar && g_DR_settings.opt_showTooFarLimitMeca ||
          actor->hStandardGame->ulCustomBits & Std_C_CustBit_NoAIWhenTooFar && g_DR_settings.opt_showTooFarLimitAI) {

          glm::vec3 dist = mainCharPos - ToGLMVec(spo->p_stGlobalMatrix->stPos);
          float totalDist = abs(dist.x) + abs(dist.y) + abs(dist.z);
          float delta = abs(limit - totalDist);

          float alpha = g_DR_settings.opt_tooFarLimitHideRange > 0 ? min(max(1.0f - (delta / g_DR_settings.opt_tooFarLimitHideRange), 0.0f), 1.0f) : 1.0f;
          if (alpha > 0.0f) {

            glm::mat4 tooFarLimitMatrix = glm::translate(glm::mat4(1.0f), ToGLMVec(spo->p_stGlobalMatrix->stPos));
            tooFarLimitMatrix = glm::scale(tooFarLimitMatrix, glm::vec3(limit * 2));

            shader->use();
            shader->setMat4("uModel", tooFarLimitMatrix);
            shader->setTex2D("tex1", Textures::Toofar, 0);
            shader->setVec3("uvScale", glm::vec3(limit * 2));
            shader->setBool("useSecondTexture", false);
            shader->setFloat("uAlphaMult", alpha);
            tooFarLimitMesh.draw(shader);
            shader->setFloat("uAlphaMult", 1.0f);
            shader->setVec3("uvScale", glm::vec3(1));
            shader->setMat4("uModel", model);
          }
        }

      }

    }

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
    shader->setBool("useSecondTexture", false);
    sphere.draw(shader);
  }

  HIE_tdstSuperObject* child;
  LST_M_DynamicForEach(spo, child) {

    if (child == g_DR_selectedObject) {
      shader->setBool("isSelected", TRUE);
    }

    renderSPO(shader, child, activeSector, view, proj, mainCharPos); // Draw all children as "selected"

    if (child == g_DR_selectedObject) {
      shader->setBool("isSelected", FALSE);
    }
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

glm::vec3 mainCharPos;

void Scene::renderPass(bool opaquePass, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {


  geometryShader->use();

  geometryShader->setTex2D("tex1", Textures::ColDefault, 0);
  geometryShader->setBool("useSecondTexture", FALSE);

  geometryShader->setBool("uOpaquePass", opaquePass);
  geometryShader->setMat4("uModel", model);
  geometryShader->setMat4("uView", view);
  geometryShader->setMat4("uProjection", proj);
  geometryShader->setFloat("uAlphaMult", 1.0f);
  geometryShader->setFloat("falloffDistance", 0.0f);

  if (GAM_g_stEngineStructure->g_hMainActor != nullptr) {
    mainCharPos = ToGLMVec(GAM_g_stEngineStructure->g_hMainActor->p_stGlobalMatrix->stPos);
  }

  renderSPO(geometryShader, *GAM_g_p_stFatherSector, true, view, proj, mainCharPos);
  renderSPO(geometryShader, *GAM_g_p_stDynamicWorld, true, view, proj, mainCharPos);
  renderSPO(geometryShader, *GAM_g_p_stInactiveDynamicWorld, false, view, proj, mainCharPos);

  DR_DLG_Utils_DrawScene(this, geometryShader);
}

void Scene::setCameraPosition(glm::vec3 from, glm::vec3 to) {
  useMouseLook = true;
  mouseLook.SetManually(from, glm::normalize(to - from)); 
}

void Scene::render(GLFWwindow * window, float display_w, float display_h) {
  assert(geometryShader != nullptr);

  generateRenderTextures(display_w, display_h);

  glViewport(0, 0, display_w, display_h);
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE); // Culling will be enabled for ZDR only
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
        ACT_Teleport(g_DR_rayman, newPos);
        *GAM_g_ucIsEdInGhostMode = true;
        wasTeleporting = true;
      }
    }
    view = glm::lookAtRH(mouseLook.position, mouseLook.position + mouseLook.forward, glm::vec3(0,0,1));
  } else {
    mouseLook.SetManually(ToGLMVec(cam->p_stGlobalMatrix->stPos), -glm::vec3(camMatrix[1]));
    view = glm::lookAtRH(mouseLook.position, mouseLook.position + mouseLook.forward, glm::vec3(camMatrix[2]));
  }

  if (!useMouseLook || !captureMouseLookInput) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (wasTeleporting) {
      wasTeleporting = false;
      *GAM_g_ucIsEdInGhostMode = false;
      DR_Cheats_SavePosition();
    }
  }

  mouseLook.Update(window);

  float aspect = (float)display_w / (float)display_h;
  float fov_y = cam->hLinkedObject.p_stActor->hCineInfo->hWork->xFocal * ((float)display_h / (float)display_w);

  glm::mat4 proj = glm::perspective(fov_y, aspect, 0.1f, 10000.0f);

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