#include <vector>
#include <glad/glad.h>

std::vector<unsigned char> LoadResourceData(HINSTANCE hInstance, int resourceID);
GLuint CreateTextureFromResource(HINSTANCE hInstance, int resourceID);
