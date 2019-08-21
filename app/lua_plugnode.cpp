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
#include "nodegraph.h"
#include <plugnode.h>
#include <perilune/perilune.h>

void lua_require_app(lua_State *L)
{
    auto top = lua_gettop(L);
    lua_newtable(L);

    static perilune::UserType<Win32Window *> win32Window;
    win32Window
        .StaticMethod("new", []() { return new Win32Window; })
        .MetaMethod(perilune::MetaKey::__gc, [](Win32Window *p) { delete p; })
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", &Win32Window::Create);
            d->Method("is_running", &Win32Window::IsRunning);
            d->Method("get_state", &Win32Window::GetState);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "window");

    static perilune::UserType<DX11Context *> dx11;
    dx11
        .StaticMethod("new", []() { return new DX11Context; })
        .MetaMethod(perilune::MetaKey::__gc, [](DX11Context *p) { delete p; })
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", &DX11Context::Create);
            d->Method("get_context", &DX11Context::GetDeviceContext);
            d->Method("new_frame", &DX11Context::NewFrame);
            d->Method("present", &DX11Context::Present);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "dx11device");

    static perilune::UserType<DX11Renderer *> dx11scene;
    dx11scene
        .StaticMethod("new", []() { return new DX11Renderer; })
        .MetaMethod(perilune::MetaKey::__gc, [](DX11Renderer *p) { delete p; })
        .MetaIndexDispatcher([](auto d) {
            d->Method("draw_teapot", &DX11Renderer::DrawTeapot);
            d->Method("set_shader", &DX11Renderer::SetShader);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "dx11scene");

    static perilune::UserType<OrbitCamera *> orbit_camera;
    orbit_camera
        .StaticMethod("new", []() { return new OrbitCamera(); })
        .MetaMethod(perilune::MetaKey::__gc, [](OrbitCamera *p) { delete p; })
        .MetaIndexDispatcher([](auto d) {
            d->Method("window_input", &OrbitCamera::WindowInput);
            d->Method("get_state", &OrbitCamera::GetState);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "orbit_camera");

    static perilune::UserType<GUI *> gui;
    gui
        .StaticMethod("new", []() { return new GUI; })
        .MetaMethod(perilune::MetaKey::__gc, [](GUI *p) { delete p; })
        .MetaIndexDispatcher([](auto d) {
            d->Method("initialize", &GUI::Initialize);
            d->Method("new_frame", &GUI::NewFrame);
            d->Method("show", &GUI::Show);
            d->Method("render", &GUI::Render);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "gui");

    lua_setglobal(L, "app");
    assert(top == lua_gettop(L));
}
