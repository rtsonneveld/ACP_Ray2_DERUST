#include "dialogs/dialogs.hpp"
#include "ui/ui.hpp"
#include "ui/settings.hpp"
#include <iostream>
#include "rendering/scene.hpp"
#include "rendering/geo_mesh.hpp"
#include "rendering/textures.hpp"

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

void DR_UI_Update()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

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
