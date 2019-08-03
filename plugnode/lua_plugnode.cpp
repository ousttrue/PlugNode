#include "lua_plugnode.h"
#include <assert.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
}

#include "win32_window.h"
#include "dx11_context.h"
#include "dx11_renderer.h"
#include "orbit_camera.h"
#include "gui.h"
#include <perilune.h>

void lua_require_plugnode(lua_State *L)
{
    auto top = lua_gettop(L);
    lua_newtable(L);

    static perilune::UserType<Win32Window *> win32Window;
    win32Window
        .StaticMethod("new", []() { return new Win32Window; })
        .Destructor([](Win32Window *p) { delete p; })
        .Method("create", &Win32Window::Create)
        .Method("is_running", &Win32Window::IsRunning)
        .Method("get_state", &Win32Window::GetState)
        .LuaNewType(L);
    lua_setfield(L, -2, "window");

    static perilune::UserType<DX11Context *> dx11;
    dx11
        .StaticMethod("new", []() { return new DX11Context; })
        .Destructor([](DX11Context *p) { delete p; })
        .Method("create", &DX11Context::Create)
        .Method("get_context", &DX11Context::GetDeviceContext)
        .Method("new_frame", &DX11Context::NewFrame)
        .Method("present", &DX11Context::Present)
        .LuaNewType(L);
    lua_setfield(L, -2, "dx11device");

    static perilune::UserType<DX11Renderer *> dx11scene;
    dx11scene
        .StaticMethod("new", []() { return new DX11Renderer; })
        .Destructor([](DX11Renderer *p) { delete p; })
        .Method("draw_teapot", &DX11Renderer::DrawTeapot)
        .LuaNewType(L);
    lua_setfield(L, -2, "dx11scene");

    static perilune::UserType<OrbitCamera *> orbit_camera;
    orbit_camera
        .StaticMethod("new", []() { return new OrbitCamera(); })
        .Destructor([](OrbitCamera *p) { delete p; })
        .Method("window_input", &OrbitCamera::WindowInput)
        .Method("get_state", &OrbitCamera::GetState)
        .LuaNewType(L);
    lua_setfield(L, -2, "orbit_camera");

    static perilune::UserType<GUI *> gui;
    gui
        .StaticMethod("new", []() { return new GUI; })
        .Destructor([](GUI *p) { delete p; })
        .Method("initialize", &GUI::Initialize)
        .Method("new_frame", &GUI::NewFrame)
        .Method("show", &GUI::Show)
        .Method("render", &GUI::Render)
        .LuaNewType(L);
    lua_setfield(L, -2, "gui");

    lua_setglobal(L, "plugnode");
    assert(top == lua_gettop(L));
}
