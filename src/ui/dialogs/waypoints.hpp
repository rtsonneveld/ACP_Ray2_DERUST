#pragma once
#include <rendering/shader.hpp>
#include <rendering/scene.hpp>

void DR_DLG_Waypoints_Init();
void DR_DLG_Waypoints_OnMapEnter();
void DR_DLG_Waypoints_Draw();
void DR_DLG_Waypoints_DrawScene(Scene* scene, Shader* shader);