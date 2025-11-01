#include <windows.h>
#include "resourceutil.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <stdexcept>

std::vector<unsigned char> LoadResourceData(HINSTANCE hInstance, int resourceID) {
  HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resourceID), L"PNG"); 
  if (!hRes) throw std::runtime_error("FindResource failed");

  HGLOBAL hMem = LoadResource(hInstance, hRes);
  if (!hMem) throw std::runtime_error("LoadResource failed");

  DWORD size = SizeofResource(hInstance, hRes);
  void* data = LockResource(hMem);
  if (!data || size == 0) throw std::runtime_error("LockResource failed");

  std::vector<unsigned char> bytes(size);
  memcpy(bytes.data(), data, size);

  return bytes;
}

GLuint CreateTextureFromResource(HINSTANCE hInstance, int resourceID) {

  auto data = LoadResourceData(hInstance, resourceID);

  int width, height, channels;
  unsigned char* pixels = stbi_load_from_memory(
    data.data(), (int)data.size(),
    &width, &height, &channels, 4
  );
  if (!pixels)
    throw std::runtime_error("Failed to decode PNG from memory");

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  stbi_image_free(pixels);
  return texID;
}