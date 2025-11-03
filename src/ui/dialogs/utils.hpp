#pragma once
#include <rendering/mesh.hpp>
#include <rendering/shader.hpp>
#include <rendering/scene.hpp>

void DR_DLG_Utils_Init();
void DR_DLG_Utils_Draw();
void DR_DLG_Utils_DrawScene(Scene * scene, Shader * shader);