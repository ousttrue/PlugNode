#include "gui.h"
#include "window_state.h"
#include "nodegraph.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

GUI::GUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

GUI::~GUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
}

void GUI::Initialize(void *hwnd, void *device, void *deviceContext)
{
    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init((ID3D11Device *)device, (ID3D11DeviceContext *)deviceContext);
}

bool GUI::NewFrame(const WindowState *windowState)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = (int)windowState->Mouse.IsDown(ButtonFlags::Left);
    io.MouseDown[1] = (int)windowState->Mouse.IsDown(ButtonFlags::Right);
    io.MouseDown[2] = (int)windowState->Mouse.IsDown(ButtonFlags::Middle);
    io.MouseWheel = (float)windowState->Mouse.Wheel;
    ImGui::NewFrame();

    // camera update
    return ImGui::IsAnyWindowHovered();
}

void GUI::Show()
{
    static bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);

    static bool showNodeGraph = true;
    ShowNodeGraph(&showNodeGraph);
}

void GUI::Render()
{
    // imgui Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
