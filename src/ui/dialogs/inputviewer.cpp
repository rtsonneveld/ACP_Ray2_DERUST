#include "ui/ui.hpp"
#include "ui/settings.hpp"

#include <ACP_Ray2.h>

constexpr float JOYSTICK_RADIUS = 55.0f;
constexpr float PADDING = 4.0f;
constexpr float KEY_SIZE = 55.0f;
constexpr float KEY_GAP = 2.0f;
constexpr float GROUP_SPACING = 20.0f;

static void DrawJoystick(ImDrawList* drawList, ImVec2 origin, IPT_tdstEntryElement* entries)
{
    const float dotRadius = 5.0f;
    ImVec2 center = ImVec2(origin.x + JOYSTICK_RADIUS + PADDING, origin.y + JOYSTICK_RADIUS + PADDING);

    float normX = entries[0].xAnalogicValue / 100.0f;
    float normY = entries[1].xAnalogicValue / 100.0f;
    float magnitude = sqrtf(normX * normX + normY * normY);

    // Clamp to avoid drawing outside the circle
    if (magnitude > 1.0f)
    {
        normX = normX / magnitude;
        normY = normY / magnitude;
    }

    // Outer circle and crosshair
    drawList->AddCircle(center, JOYSTICK_RADIUS, IM_COL32(150, 150, 150, 255), 64, 2.0f);
    drawList->AddLine(
        ImVec2(center.x - JOYSTICK_RADIUS, center.y),
        ImVec2(center.x + JOYSTICK_RADIUS, center.y),
        IM_COL32(80, 80, 80, 255), 1.0f);
    drawList->AddLine(
        ImVec2(center.x, center.y - JOYSTICK_RADIUS),
        ImVec2(center.x, center.y + JOYSTICK_RADIUS),
        IM_COL32(80, 80, 80, 255), 1.0f);

    // Joystick position
    ImVec2 dotPos = ImVec2(center.x + normX * JOYSTICK_RADIUS, center.y + normY * JOYSTICK_RADIUS);
    drawList->AddLine(center, dotPos, IM_COL32(100, 200, 255, 180), 1.5f);
    drawList->AddCircleFilled(dotPos, dotRadius, IM_COL32(100, 200, 255, 255));
    drawList->AddCircle(dotPos, dotRadius, IM_COL32(255, 255, 255, 200));

    // Display X and Y values
    char xLabel[32];
    char yLabel[32];
    snprintf(xLabel, sizeof(xLabel), "X: %.2f", entries[0].xAnalogicValue);
    snprintf(yLabel, sizeof(yLabel), "Y: %.2f", entries[1].xAnalogicValue);

    drawList->AddText(
        ImVec2(center.x - JOYSTICK_RADIUS, center.y + JOYSTICK_RADIUS + 5.0f), 
        IM_COL32(180, 180, 180, 255), 
        xLabel);
    drawList->AddText(
        ImVec2(center.x - JOYSTICK_RADIUS, center.y + JOYSTICK_RADIUS + 5.0f + ImGui::GetTextLineHeight()), 
        IM_COL32(180, 180, 180, 255), 
        yLabel);
}

static void DrawInput(ImDrawList* drawList, ImVec2 keysOrigin, int col, int row, bool pressed, const char* label)
{
    const float step = KEY_SIZE + KEY_GAP;
    const float rounding = 3.0f;

    ImVec2 topLeft = ImVec2(keysOrigin.x + col * step, keysOrigin.y + row * step);
    ImVec2 bottomRight = ImVec2(topLeft.x + KEY_SIZE, topLeft.y + KEY_SIZE);

    ImU32 bgColor = pressed ? IM_COL32(100, 200, 255, 255) : IM_COL32(60, 60, 60, 255);
    ImU32 txtColor = pressed ? IM_COL32(0, 0, 0, 255) : IM_COL32(180, 180, 180, 255);

    drawList->AddRectFilled(topLeft, bottomRight, bgColor, rounding);
    drawList->AddRect(topLeft, bottomRight, IM_COL32(120, 120, 120, 255), rounding);

    ImVec2 txtSize = ImGui::CalcTextSize(label);
    drawList->AddText(
        ImVec2(topLeft.x + (KEY_SIZE - txtSize.x) * 0.5f, topLeft.y + (KEY_SIZE - txtSize.y) * 0.5f),
        txtColor, label);
}

static void DrawInputs(ImDrawList* drawList, ImVec2 origin, float offsetX, IPT_tdstEntryElement* entries)
{
    ImVec2 keysOrigin = ImVec2(origin.x + offsetX, origin.y + PADDING);

    // Directions
    DrawInput(drawList, keysOrigin, 1, 0, entries[2].lState > 0, "^");
    DrawInput(drawList, keysOrigin, 0, 1, entries[5].lState > 0, "<");
    DrawInput(drawList, keysOrigin, 1, 1, entries[3].lState > 0, "v");
    DrawInput(drawList, keysOrigin, 2, 1, entries[4].lState > 0, ">");

    ImVec2 group2Origin = ImVec2(keysOrigin.x + (KEY_SIZE + KEY_GAP) * 3.0f + GROUP_SPACING, keysOrigin.y);

    DrawInput(drawList, group2Origin, 0, 0, entries[17].lState > 0, "Cam L");
    DrawInput(drawList, group2Origin, 1, 0, entries[16].lState > 0, "Cam R");
    DrawInput(drawList, group2Origin, 2, 0, entries[19].lState > 0, "Cam\nreset");

    DrawInput(drawList, group2Origin, 0, 1, entries[13].lState > 0, "Strafe");
    DrawInput(drawList, group2Origin, 1, 1, entries[11].lState > 0, "Shoot");
    DrawInput(drawList, group2Origin, 2, 1, entries[10].lState > 0, "Jump");

    DrawInput(drawList, group2Origin, 0, 2, entries[22].lState > 0, "Think");
    DrawInput(drawList, group2Origin, 1, 2, entries[14].lState > 0, "Dive");
    DrawInput(drawList, group2Origin, 2, 2, entries[6].lState > 0, "Walk");

    ImVec2 group3Origin = ImVec2(group2Origin.x + (KEY_SIZE + KEY_GAP) * 3.0f + GROUP_SPACING, keysOrigin.y);

    DrawInput(drawList, group3Origin, 0, 0, entries[7].lState > 0, "Pause");
    DrawInput(drawList, group3Origin, 1, 0, entries[9].lState > 0, "Accept");
    DrawInput(drawList, group3Origin, 2, 0, entries[8].lState > 0, "Cancel");

    DrawInput(drawList, group3Origin, 0, 1, entries[18].lState > 0, "Look");
    DrawInput(drawList, group3Origin, 1, 1, entries[24].lState > 0, "Hud");
    DrawInput(drawList, group3Origin, 2, 1, entries[116].lState > 0, "Screen\nshot");
}

void DR_DLG_InputViewer_Draw()
{
    if (!g_DR_settings.dlg_inputviewer) return;

    ImGuiWindowClass windowClass;
    windowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoTaskBarIcon;
    ImGui::SetNextWindowClass(&windowClass);

    if (ImGui::Begin("Input Viewer", &g_DR_settings.dlg_inputviewer, ImGuiWindowFlags_NoCollapse))
    {
        IPT_tdstEntryElement* entries = IPT_g_stInputStructure->d_stEntryElementArray;

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 origin = ImGui::GetCursorScreenPos();

        const float joystickAreaWidth = JOYSTICK_RADIUS * 2.0f + PADDING * 2.0f + GROUP_SPACING;

        DrawJoystick(drawList, origin, entries);
        DrawInputs(drawList, origin, joystickAreaWidth, entries);

        // Allocate enough space for the drawn elements
        ImGui::Dummy(ImVec2(700.0f, 180.0f));
    }

    ImGui::End();
}