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
GLFWwindow* window = nullptr;
HWND hWindow = nullptr;
HWND hWndR2 = nullptr;

ImGuiIO* io;

ImVec2 lastCentralNodePos;
ImVec2 lastCentralNodeSize;

// 3D Scene
Scene scene;


WNDPROC WndProcGLFW = NULL;	

LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_KEYDOWN:
			SendMessage(hWndR2, WM_KEYDOWN, wParam, lParam);
			break;

		case WM_CHAR:
			SendMessage(hWndR2, WM_CHAR, wParam, lParam);
			break;

		case WM_CLOSE:
			SendMessage(hWndR2, WM_CLOSE, 0, 0);
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			return 0;

		case WM_DR_INITWND:
		{
			std::wstring newTitle = L"DERUST // ";
			wchar_t szTitle[64];
			GetWindowText(hWndR2, szTitle, 64);
			newTitle += szTitle;
			SetWindowText(hWnd, newTitle.c_str());

			SetWindowPos(hWndR2, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

			return 0;
		}
	}

	return CallWindowProc(WndProcGLFW, hWnd, uMsg, wParam, lParam);
}


int DR_UI_Init(HWND a_window_r2, HMODULE module)
{
	if (ui_initialized) {
		return 0;
	}

	hWndR2 = a_window_r2;

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Transparent window

	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	window = glfwCreateWindow(1280, 1024, "DERUST Main Window", NULL, NULL);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	hWindow = glfwGetWin32Window(window);

	WndProcGLFW = (WNDPROC)GetWindowLongPtr(hWindow, GWLP_WNDPROC);
	SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)NewWndProc);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Init OpenGL loader (GLAD)
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Textures::LoadAllTextures(module);

	scene.init();

	// Load settings from file
	DR_LoadSettings();
	DR_DLG_Init(hWndR2);

	SetParent(hWndR2, hWindow);

	SetWindowPos(hWndR2, NULL, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

  // Hide the Rayman 2 window by setting a 1x1 pixel region in the center
	RECT rc;
	GetClientRect(hWndR2, &rc);

	int cx = (rc.right - rc.left) / 2;
	int cy = (rc.bottom - rc.top) / 2;

	HRGN rgn = CreateRectRgn(cx, cy, cx + 1, cy + 1);

	SetWindowRgn(hWndR2, rgn, TRUE);

	ui_initialized = 1;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	ImVec2 pos(10, ImGui::GetIO().DisplaySize.y - 20);
	drawList->AddText(pos, IM_COL32(255, 255, 255, 255),
		"Initializing game...");

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	glfwSwapBuffers(window);
	return 0;
}

void DR_UI_OnMapExit() {
  GeometricObjectMesh::clearCache();
}

// Viewport texture
GLuint vp_texture;
GLuint vp_PBO;
unsigned int vp_width = -1;
unsigned int vp_height = -1;

extern "C" HANDLE g_hAFrameIsWaiting;
extern "C" HANDLE g_hFrameDoneCopying;

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

	GLD_tdstViewportAttributes stViewportAttr;
	BOOL bCanWrite;
	GLD_bRequestWriteToViewport2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport, &stViewportAttr, &bCanWrite);

	if (!bCanWrite) {
		GLD_bWriteToViewportFinished2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport);
		ReleaseSemaphore(g_hFrameDoneCopying, 1, NULL);
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

	GLD_bWriteToViewportFinished2D(GAM_g_stEngineStructure->hGLDDevice, GAM_g_stEngineStructure->hGLDViewport);
	ReleaseSemaphore(g_hFrameDoneCopying, 1, NULL);

	glBindTexture(GL_TEXTURE_2D, vp_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dwWidth, dwHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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

void DR_UI_Update() {

	// Request new data
	WaitForSingleObject(g_hAFrameIsWaiting, INFINITE);

	glfwPollEvents();
	if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
	{
		ImGui_ImplGlfw_Sleep(10);
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	UpdateViewportTexture();

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

	DR_DLG_Draw(hWndR2);

	ShowTextureWindow(vp_texture, vp_width, vp_height);

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

	DR_SaveSettings();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}