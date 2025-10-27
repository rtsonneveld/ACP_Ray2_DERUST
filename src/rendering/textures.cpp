#include "textures.hpp"
#include "resourceutil.hpp"
#include "resource.h"
#include <iostream>
#include <vector>
#include <stdexcept>

namespace Textures {
  static std::vector<std::pair<GLuint*, int>> g_registeredTextures;

  std::vector<GLuint> textureTableZDM;
  std::vector<GLuint> textureTableZDE;
  std::vector<GLuint> textureTableZDR;
  std::vector<GLuint> textureTableZDD;

#define TEXTURE(name, resourceId) \
  GLuint name; \
  namespace { struct name##_registrar { \
    name##_registrar() { g_registeredTextures.emplace_back(&name, resourceId); } \
  } name##_instance; }

#include "textures_list.hpp"
#undef TEXTURE

  void LoadAllTextures(HINSTANCE hInst) {
    try {
      for (auto& tex : g_registeredTextures) {
        *(tex.first) = CreateTextureFromResource(hInst, tex.second);
      }

      textureTableZDM = {
        Textures::ColZdmFeet,
        Textures::ColZdmBody,
        Textures::ColZdmHead,
        Textures::ColZdmHand,
        Textures::ColZdmTraversesol,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColSwim,
        Textures::ColUnknown,
      };


      textureTableZDR = {
        Textures::ColSlippery,
        Textures::ColBounce,
        Textures::ColLedgegrab,
        Textures::ColUser1,
        Textures::ColGrappinbis,
        Textures::ColGi,
        Textures::ColClimb,
        Textures::ColElectric,
        Textures::ColLava,
        Textures::ColFalldeath,
        Textures::ColPaf,
        Textures::ColMort,
        Textures::ColUser2,
        Textures::ColUser3,
        Textures::ColWater,
        Textures::ColNocol,
      };

      textureTableZDD = {
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
      };

      textureTableZDE = {
        Textures::ColZdeTakedamage,
        Textures::ColZdeDodamage,
        Textures::ColUnknown,
        Textures::ColZdeGrapple,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
        Textures::ColUnknown,
      };

      std::cout << "All textures loaded successfully.\n";
    }
    catch (const std::exception& e) {
      std::cerr << "Texture loading failed: " << e.what() << "\n";
    }
  }

  void FreeAllTextures() {
    for (auto& tex : g_registeredTextures)
      glDeleteTextures(1, tex.first);
    g_registeredTextures.clear();
  }
}
