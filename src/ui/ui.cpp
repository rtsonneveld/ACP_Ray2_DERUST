#include "dialogs/dialogs.hpp"
#include "ui/ui.hpp"
#include <iostream>
#include "rendering/scene.h"

// C INCLUDE
#include "ui/ui_bridge.h"

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// State
char ui_initialized = 0;

// Handles
GLFWwindow* window;
ImGuiIO* io;
HWND window_r2;

ImVec2 lastCentralNodePos;
ImVec2 lastCentralNodeSize;

// 3D Scene
Scene scene;

// DEBUG
bool dbg_drawCollision = false;
bool dbg_drawVisuals = true;

// Main code
int DR_UI_Init(HWND a_window_r2)
{
  if (ui_initialized) {
    return 0;
  }

  window_r2 = a_window_r2;

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

  // Transparent window

  auto monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  window = glfwCreateWindow(1280, 1024, "DERUST Main Window", NULL, NULL);
  if (window == nullptr)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  io = &ImGui::GetIO(); (void)io;
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  LONG lStyle = GetWindowLong(a_window_r2, GWL_STYLE);
  lStyle &= ~(WS_BORDER | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX);
  lStyle |= WS_CHILD;
  SetWindowLong(a_window_r2, GWL_STYLE, lStyle);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Init OpenGL loader (GLAD)
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  scene.init();

  ui_initialized = 1;
  return 0;
}

void DR_UI_Update() {

  glfwPollEvents();
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
  {
    ImGui_ImplGlfw_Sleep(10);
    return;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  
  ImGuiID id = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
  ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(id);

  // Background text
  {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 pos(10, ImGui::GetIO().DisplaySize.y - 20);
    ImU32 color = IM_COL32(255, 255, 255, 255);
    drawList->AddText(pos, color, "RMB for mouselook, ESC to reset camera");
  }

  DR_DLG_Draw(window_r2);

  ImGui::Checkbox("Draw visuals", &dbg_drawVisuals);
  ImGui::Checkbox("Draw collision", &dbg_drawCollision);
  if (ImGui::Button("Toggle")) {
    dbg_drawVisuals = !dbg_drawVisuals;
    dbg_drawCollision = !dbg_drawCollision;
  }

  ImGui::Render();

  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);

  scene.render(window, display_w, display_h);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  glfwSwapBuffers(window);
}

void DR_UI_DeInit() {

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}