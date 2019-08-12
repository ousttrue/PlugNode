#include "gui.h"
#include "window_state.h"
#include "context.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Windows.h>

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

static bool ImGui_ImplWin32_UpdateMouseCursor()
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:
            win32_cursor = IDC_ARROW;
            break;
        case ImGuiMouseCursor_TextInput:
            win32_cursor = IDC_IBEAM;
            break;
        case ImGuiMouseCursor_ResizeAll:
            win32_cursor = IDC_SIZEALL;
            break;
        case ImGuiMouseCursor_ResizeEW:
            win32_cursor = IDC_SIZEWE;
            break;
        case ImGuiMouseCursor_ResizeNS:
            win32_cursor = IDC_SIZENS;
            break;
        case ImGuiMouseCursor_ResizeNESW:
            win32_cursor = IDC_SIZENESW;
            break;
        case ImGuiMouseCursor_ResizeNWSE:
            win32_cursor = IDC_SIZENWSE;
            break;
        case ImGuiMouseCursor_Hand:
            win32_cursor = IDC_HAND;
            break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return true;
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

    if (windowState->Flags & 0x01)
    {
        ImGui_ImplWin32_UpdateMouseCursor();
    }

    ImGui::NewFrame();

    // camera update
    return ImGui::IsAnyWindowHovered() || plugnode::Context::s_popup;
}

void GUI::Show()
{
    static bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);
}

void GUI::Render()
{
    // imgui Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
