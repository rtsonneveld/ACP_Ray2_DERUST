#include "dialogs/dialogs.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include <iostream>
#include "rendering/scene.hpp"
#include "rendering/geo_mesh.hpp"
#include "rendering/textures.hpp"
#include <mutex>
#include <fstream>

// C INCLUDE
#include "ui/ui_bridge.h"
#include <ACP_Ray2.h>
#include "mod/globals.h"
#include <time.h>

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// State
char ui_initialized = 0;

// Handles
HWND   window_r2;
HDC    g_hDC = nullptr;
HGLRC  g_hGLRC = nullptr;

ImGuiIO* io;

ImVec2 lastCentralNodePos;
ImVec2 lastCentralNodeSize;

// 3D Scene
Scene scene;

// Viewport texture
GLuint vp_texture;
GLuint vp_PBO;
unsigned int vp_width = -1;
unsigned int vp_height = -1;

static bool CreateOpenGLContext(HWND hwnd)
{
  g_hDC = GetDC(hwnd);
  if (!g_hDC)
    return false;

  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 24;
  pfd.cStencilBits = 8;

  int pf = ChoosePixelFormat(g_hDC, &pfd);
  if (!pf || !SetPixelFormat(g_hDC, pf, &pfd))
    return false;

  g_hGLRC = wglCreateContext(g_hDC);
  if (!g_hGLRC)
    return false;

  if (!wglMakeCurrent(g_hDC, g_hGLRC))
    return false;

  return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool DR_UI_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
  {
    // If ImGui handled it, optionally swallow it
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
      return TRUE;
  }

  return FALSE;
}

int DR_UI_Init(HWND a_window_r2, HMODULE module)
{
  if (ui_initialized)
    return 0;

  window_r2 = a_window_r2;

  if (!CreateOpenGLContext(window_r2))
    return 1;

  // Load OpenGL
  if (!gladLoadGL())
    return 1;

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImNodes::CreateContext();

  io = &ImGui::GetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  // Initialize ImGui backends
  ImGui_ImplWin32_Init(window_r2);
  if (!ImGui_ImplOpenGL3_Init("#version 130")) {
    MessageBox(nullptr, L"ImGui_ImplOpenGL3_Init failed", L"Error", MB_OK);
    return 1;
  }

  Textures::LoadAllTextures(module);
  scene.init();

  DR_LoadSettings();
  DR_DLG_Init(window_r2);

  ui_initialized = 1;
  return 0;
}


void DR_UI_OnMapExit() {
  GeometricObjectMesh::clearCache();
}

void UpdateViewportTexture() {
  DWORD dwWidth = GAM_g_stEngineStructure->stViewportAttr.dwWidth;
  DWORD dwHeight = GAM_g_stEngineStructure->stViewportAttr.dwHeight;

  // Resize texture if needed
  if (vp_width != dwWidth || vp_height != dwHeight) {
    if (vp_texture) glDeleteTextures(1, &vp_texture);
    glGenTextures(1, &vp_texture);
    glBindTexture(GL_TEXTURE_2D, vp_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, dwWidth, dwHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    if (vp_PBO) glDeleteBuffers(1, &vp_PBO);
    glGenBuffers(1, &vp_PBO);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vp_PBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, dwWidth * dwHeight * 4, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    vp_width = dwWidth;
    vp_height = dwHeight;
  }

  // Request new data
  GLD_tdstViewportAttributes stViewportAttr;
  BOOL bCanWrite;
  GLD_bRequestWriteToViewport2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport, &stViewportAttr, &bCanWrite);

  if (!bCanWrite) {
    GLD_bWriteToViewportFinished2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport);
    return;
  }

  // Upload via PBO
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vp_PBO);
  void* gpuPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, dwWidth * dwHeight * 4, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

  if (gpuPtr) {
    uint8_t* dst = (uint8_t*)gpuPtr;
    uint16_t* src = (uint16_t*)stViewportAttr.p_cVirtualScreen;

    for (DWORD i = 0; i < dwWidth * dwHeight; i++) {
      uint16_t pixel = src[i];
      // Extract RGB565 components
      uint8_t r = (pixel >> 11) & 0x1F;
      uint8_t g = (pixel >> 5) & 0x3F;
      uint8_t b = pixel & 0x1F;

      // Convert to 8-bit per channel
      dst[i * 4 + 0] = (r << 3) | (r >> 2); // Red
      dst[i * 4 + 1] = (g << 2) | (g >> 4); // Green
      dst[i * 4 + 2] = (b << 3) | (b >> 2); // Blue
      dst[i * 4 + 3] = 255;                 // Alpha
    }

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  }

  glBindTexture(GL_TEXTURE_2D, vp_texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dwWidth, dwHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  GLD_bWriteToViewportFinished2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport);
}

void ShowTextureWindow(GLuint vp_texture, int tex_width, int tex_height) {
  ImGui::Begin("Viewport Texture");

  // Get available size in the window
  ImVec2 avail_size = ImGui::GetContentRegionAvail();

  // Compute scaling factor to fit the window while keeping aspect ratio
  float scale_x = avail_size.x / tex_width;
  float scale_y = avail_size.y / tex_height;
  float scale = (scale_x < scale_y) ? scale_x : scale_y; // choose smaller scale

  // Calculate final display size
  ImVec2 display_size = ImVec2(tex_width * scale, tex_height * scale);

  // Center the image in the window
  ImVec2 cursor_pos = ImGui::GetCursorPos();
  ImVec2 offset = ImVec2((avail_size.x - display_size.x) * 0.5f,
    (avail_size.y - display_size.y) * 0.5f);
  ImGui::SetCursorPos(ImVec2(cursor_pos.x + offset.x, cursor_pos.y + offset.y));

  // Display the texture
  ImGui::Image((ImTextureID)(uintptr_t)vp_texture, display_size);

  ImGui::End();
}


void DR_UI_Update()
{
  static std::mutex render_mutex;
  std::lock_guard<std::mutex> lock(render_mutex);

  if (!ui_initialized) {
    return;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  UpdateViewportTexture();

  ImGuiID id = ImGui::DockSpaceOverViewport(
    0, nullptr,
    ImGuiDockNodeFlags_NoDockingInCentralNode |
    ImGuiDockNodeFlags_PassthruCentralNode);

  // Background text
  {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 pos(10, ImGui::GetIO().DisplaySize.y - 20);
    drawList->AddText(pos, IM_COL32(255, 255, 255, 255),
      "RMB for mouselook, ESC to reset camera");
  }

  DR_DLG_Draw(window_r2);

  ShowTextureWindow(vp_texture, vp_width, vp_height);

  ImGui::Render();

  RECT rc;
  GetClientRect(window_r2, &rc);
  int display_w = rc.right - rc.left;
  int display_h = rc.bottom - rc.top;

  scene.render(nullptr, display_w, display_h);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SwapBuffers(g_hDC);
}

void DR_UI_DeInit()
{
  DR_SaveSettings();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();

  ImPlot::DestroyContext();
  ImNodes::DestroyContext();
  ImGui::DestroyContext();

  if (g_hGLRC)
  {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(g_hGLRC);
    g_hGLRC = nullptr;
  }

  if (g_hDC)
  {
    ReleaseDC(window_r2, g_hDC);
    g_hDC = nullptr;
  }
}
