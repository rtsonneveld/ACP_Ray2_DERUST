#include "waypoints.hpp"
#include "ui/ui.hpp"
#include "ui/custominputs.hpp"
#include "ui/settings.hpp"
#include "rendering/textures.hpp"
#include "mod/globals.h"
#include <ACP_Ray2.h>
#include <mod/cpa_functions.h>
#include <mod/util.h>
#include <unordered_set>

std::unordered_set<WP_tdstGraph*> knownGraphs;
std::unordered_set<WP_tdstWayPoint*> knownWaypoints;

Mesh waypointSphere;
Mesh waypointLine;
Mesh waypointQuad;

void DR_DLG_Waypoints_Init() {
  waypointSphere = Mesh::createSphere(1.0f, glm::vec3(0, 0, 0));
  waypointLine = Mesh::createCube(glm::vec3(1,1,1), glm::vec3(0, 0, 0));
  waypointQuad = Mesh::createQuad(100.0f, 100.0f);
}

void DR_DLG_Waypoints_OnMapEnter() {
  knownGraphs.clear();
  knownWaypoints.clear();
  
  std::unordered_set<AI_tdstAIModel*> aiModels;

  HIE_M_ForEachActor(actorSPO) {

    HIE_tdstEngineObject* actor = actorSPO->hLinkedObject.p_stActor;

    // Source known graphs from MSWay
    if (actor->hMSWay != nullptr && actor->hMSWay->m_hPath != nullptr) {
      knownGraphs.insert(actor->hMSWay->m_hPath);
    }

    // Source known graphs from scripts
    if (actor->hBrain != nullptr && actor->hBrain->p_stMind != nullptr) {
      AI_tdstMind* mind = actor->hBrain->p_stMind;
      if (mind->p_stAIModel != nullptr) {
        AI_tdstAIModel* aiModel = mind->p_stAIModel;
        aiModels.insert(aiModel);

        if (aiModel->p_stDsgVar != nullptr) {
          for (int i = 0;i < aiModel->p_stDsgVar->ucNbDsgVar;i++) {
            if (aiModel->p_stDsgVar->a_stDsgVarInfo[i].eTypeId == AI_E_dvt_Graph) {
              WP_tdstGraph* graph = *(WP_tdstGraph**)ACT_DsgVarPtr(actor, i);

              if (graph != nullptr) {
                knownGraphs.insert(graph);
              }
            }

            if (aiModel->p_stDsgVar->a_stDsgVarInfo[i].eTypeId == AI_E_dvt_WayPoint) {
              WP_tdstWayPoint* waypoint = *(WP_tdstWayPoint**)ACT_DsgVarPtr(actor, i);

              if (waypoint != nullptr) {
                knownWaypoints.insert(waypoint);
              }
            }

            if (aiModel->p_stDsgVar->a_stDsgVarInfo[i].eTypeId == AI_E_dvt_WayPointArray) {

              AI_tdstArray waypointArray = *(AI_tdstArray*)ACT_DsgVarPtr(actor, i);

              for (int i = 0;i < waypointArray.ucMaxSize;i++) {
                WP_tdstWayPoint* waypoint = ((WP_tdstWayPoint**)waypointArray.d_ArrayElement)[i];
                if (waypoint != nullptr) {
                  knownWaypoints.insert(waypoint);
                }
              }
            }
          }
        }
      }
    }
  }

  /*for (AI_tdstAIModel* aiModel : aiModels) {
    aiModel->
  }*/
}

void DR_DLG_Waypoints_Draw() {

  if (!g_DR_settings.dlg_waypoints) return;

  if (ImGui::Begin("Waypoints", &g_DR_settings.dlg_waypoints, ImGuiWindowFlags_NoCollapse)) {
    ImGui::Checkbox("Show waypoints", &g_DR_settings.opt_drawWaypoints);
    ImGui::Checkbox("Show wp radius/arc weights", &g_DR_settings.opt_drawWaypointsRadius);
    ImGui::Checkbox("Show arcs", &g_DR_settings.opt_drawArcs);

    
    for(WP_tdstGraph* graph : knownGraphs) {
      ImGui::Text("Graph at 0x%X, %s, %s", (unsigned long)graph, graph->m_szNameOfGraph, graph->m_szReferenceSection);
    }

    for (WP_tdstWayPoint* waypoint : knownWaypoints) {
      ImGui::Text("Waypoint at 0x%X, radius %f", (unsigned long)waypoint, waypoint->m_xRadius);
    }

  }
  ImGui::End();
}

static void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture, float thickness)
{
  if (glm::distance(A, B) > 0.001f) {
    glm::vec3 dir = glm::normalize(B - A);
    float lineLength = glm::length(B - A);

    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), dir, glm::vec3(0, 1, 0)));

    glm::mat4 lineMat = glm::mat4(1.0f);
    lineMat = glm::translate(lineMat, A);
    lineMat = lineMat * rotation;

    lineMat = glm::translate(lineMat, glm::vec3(0.0f, 0.0f, -lineLength * 0.5f));
    lineMat = glm::scale(lineMat, glm::vec3(thickness, thickness, lineLength));

    shader->setMat4("uModel", lineMat);
    shader->setTex2D("tex1", texture, 0);
    shader->setTex2D("tex2", texture, 0);
    waypointLine.draw();
  }
}

void DrawWaypoint(Scene* scene, Shader* shader, WP_tdstWayPoint waypoint, bool scaleToRadius) {

  using glm::vec3;
  using glm::mat4;

  mat4 matrix;

  matrix = glm::translate(mat4(1.0f), ToGLMVec(waypoint.m_stVertex));
  float radius = 0.5f;
  if (scaleToRadius && waypoint.m_xRadius > 0.01f) {
    radius = waypoint.m_xRadius;
  }
  matrix = glm::scale(matrix, glm::vec3(radius));

  shader->use();
  shader->setMat4("uModel", matrix);
  shader->setTex2D("tex1", Textures::Waypoint, 0);
  shader->setVec3("uvScale", glm::vec3(1,1,1));
  shader->setFloat("uAlphaMult", 0.5f);

  waypointSphere.draw(shader);

  shader->setVec3("uvScale", glm::vec3(1.0f));
  shader->setFloat("uAlphaMult", 1.0f);
}

void DR_DLG_Waypoints_DrawScene(Scene* scene, Shader* shader) {
  if (!g_DR_settings.opt_drawWaypoints) {
    return;
  }

  for(WP_tdstGraph* graph : knownGraphs) {

    WP_tdstGraphNode* node;
    int index;
    
    LST_M_DynamicForEachIndex(&(graph->m_hListOfNode), node, index) {
      DrawWaypoint(scene, shader, *node->m_hWayPoint, g_DR_settings.opt_drawWaypointsRadius);
      
      if (!g_DR_settings.opt_drawArcs) continue;

      WP_tdstArc* arc;
      
      LST_M_DynamicForEach(&(node->m_hListOfArc->m_hArc), arc) {
        float thickness = 0.1f;

        if (g_DR_settings.opt_drawWaypointsRadius) {
          thickness = max(0.05f, arc->m_lWeight * 0.02f);
        }

        DrawLine(shader, ToGLMVec(node->m_hWayPoint->m_stVertex), ToGLMVec(arc->m_hNode->m_hWayPoint->m_stVertex), Textures::White, thickness);
      }
    }
  }

  for(WP_tdstWayPoint* waypoint : knownWaypoints) {
    DrawWaypoint(scene, shader, *waypoint, g_DR_settings.opt_drawWaypointsRadius);
  }
}