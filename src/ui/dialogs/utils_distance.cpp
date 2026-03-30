#include "utils_distance.hpp"

#include "utils.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"
#include "ui/settings.hpp"
#include "ui/nameLookup.hpp"
#include <ACP_Ray2.h>

// C INCLUDE
#include "mod/globals.h"
#include "mod/dsgvarnames.h"
#include "mod/ai_distancechecks.h"
#include "mod/ai_strings.h"
#include "mod/util.h"

#include <rendering/textures.hpp>
#include <rendering/shader.hpp>
#include <rendering/mesh.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace {
  HIE_tdstEngineObject* distancePersoFrom = nullptr;
  HIE_tdstEngineObject* distancePersoTo = nullptr;
  unsigned long distanceFunction = 0;

  AI_tdstNodeInterpret fakeNode;
  AI_tdstNodeInterpret fakeNode2;
  AI_tdstNodeInterpret fakeNode3;

  AI_tdstGetSetParam result;

  Mesh distanceSphere;
}

#define DISTFUNC_OFFSET 3
#define DISTFUNC_COUNT 14

void DR_DLG_Utils_Init_Distance() {
  distanceSphere = Mesh::createSphere();
}

void DR_DLG_Utils_DrawTab_Distance() {

  if (distancePersoFrom != nullptr && distancePersoFrom->hStandardGame == nullptr) {
    distancePersoFrom = nullptr;
  }
  if (distancePersoTo != nullptr && distancePersoTo->hStandardGame == nullptr) {
    distancePersoTo = nullptr;
  }
  InputPerso("Perso from##distancePersoFrom", &distancePersoFrom);
  InputPerso("Perso to##distancePersoTo", &distancePersoTo);
  DrawStringCombo("Distance function", AI_FunctionStrings + DISTFUNC_OFFSET, DISTFUNC_COUNT, (unsigned long*)&distanceFunction);

  if (distancePersoFrom == nullptr || distancePersoTo == nullptr) {
    ImGui::Text("Select two objects to measure distance between");
    return;
  }

  AI_tdeFuncId distanceFuncID = (AI_tdeFuncId)(distanceFunction + DISTFUNC_OFFSET);

  fakeNode.eType = AI_E_ti_Function;
  fakeNode.uParam.ulValue = distanceFuncID;
  fakeNode.ucDepth = 1;
  fakeNode.uwNodeToSkip = 1;

  fakeNode2.eType = AI_E_ti_PersoRef;
  fakeNode2.uParam.hActor = distancePersoTo;
  fakeNode2.ucDepth = 2;
  fakeNode2.uwNodeToSkip = 1;

  fakeNode3.eType = AI_E_ti_EndTree;
  fakeNode3.ucDepth = 1;
  fakeNode3.uwNodeToSkip = 1;

  AI_fn_p_stEvalTree(distancePersoFrom->hStandardGame->p_stSuperObject, &fakeNode, &result);
  float dist = result.uParam.xValue;
  ImGui::Text("Distance: %.3f", dist);
}

void DrawDistanceChecks(Shader* shader) {

  using glm::vec3;
  using glm::mat4;

  mat4 matrix;

  for (int i = 0; i < DR_DistanceCheckCount; i++) {

    DistanceCheckEntry entry = distanceChecks[i];
    matrix = glm::translate(mat4(1.0f), glm::vec3(entry.x, entry.y, entry.z));
    matrix = glm::scale(matrix, glm::vec3(entry.radius, entry.radius, entry.radius));

    shader->use();
    shader->setMat4("uModel", matrix);
    shader->setTex2D("tex1", entry.lifetime == 0 ? Textures::ColAi : Textures::ColAiInactive, 0);
    shader->setVec3("uvScale", glm::vec3(entry.radius));
    if (g_DR_settings.opt_distanceCheckVisibility == DistanceCheckVisibility::FadeOut) {
      float lifeFactor = (float)entry.lifetime / DR_DISTANCECHECKS_MAXLIFETIME;
      shader->setFloat("uAlphaMult", 0.5f - lifeFactor * 0.5f);
    } else {
      shader->setFloat("uAlphaMult", 0.5f);
    }

    distanceSphere.draw();

    shader->setVec3("uvScale", glm::vec3(1.0f));
    shader->setFloat("uAlphaMult", 1.0f);
  }
}