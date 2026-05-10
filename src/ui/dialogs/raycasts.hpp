#pragma once
#include <rendering/shader.hpp>
#include <rendering/scene.hpp>

void DR_DLG_Raycasts_Init();
void DR_DLG_Raycasts_Draw();
void DR_DLG_Raycasts_DrawScene(Scene* scene, Shader* shader);