#pragma once
#include <glad/glad.h>
#include <Windows.h>
#include <vector>

namespace Textures {

  extern std::vector<GLuint> textureTableZDM;
  extern std::vector<GLuint> textureTableZDE;
  extern std::vector<GLuint> textureTableZDR;
  extern std::vector<GLuint> textureTableZDD;

  void LoadAllTextures(HINSTANCE hInst);
  void FreeAllTextures();
}

// ───────────────────────────────
// Declare textures globally
// ───────────────────────────────
namespace Textures {
#define TEXTURE(name, resourceId) extern GLuint name;
#include "textures_list.hpp"
#undef TEXTURE
}
