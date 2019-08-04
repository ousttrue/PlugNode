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
#include <plugnode.h>
#include <perilune.h>

void lua_require_plugnode(lua_State *L)
{
    auto top = lua_gettop(L);
    lua_newtable(L);

    static perilune::UserType<Win32Window *> win32Window;
    win32Window
        .StaticMethod("new", []() { return new Win32Window; })
        .MetaMethod(perilune::MetaKey::__gc, [](Win32Window *p) { delete p; })
        .IndexDispatcher([](auto d) {
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
        .IndexDispatcher([](auto d) {
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
        .IndexDispatcher([](auto d) {
            d->Method("draw_teapot", &DX11Renderer::DrawTeapot);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "dx11scene");

    static perilune::UserType<OrbitCamera *> orbit_camera;
    orbit_camera
        .StaticMethod("new", []() { return new OrbitCamera(); })
        .MetaMethod(perilune::MetaKey::__gc, [](OrbitCamera *p) { delete p; })
        .IndexDispatcher([](auto d) {
            d->Method("window_input", &OrbitCamera::WindowInput);
            d->Method("get_state", &OrbitCamera::GetState);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "orbit_camera");

    static perilune::UserType<GUI *> gui;
    gui
        .StaticMethod("new", []() { return new GUI; })
        .MetaMethod(perilune::MetaKey::__gc, [](GUI *p) { delete p; })
        .IndexDispatcher([](auto d) {
            d->Method("initialize", &GUI::Initialize);
            d->Method("new_frame", &GUI::NewFrame);
            d->Method("show", &GUI::Show);
            d->Method("render", &GUI::Render);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "gui");

    using namespace plugnode;
    static perilune::UserType<std::vector<std::string> *> stringList;
    perilune::AddDefaultMethods(stringList);
    stringList
        .LuaNewType(L);
    lua_setfield(L, -2, "string_list");

    static perilune::UserType<NodeDefinition *> nodeDefinition;
    nodeDefinition
        .IndexDispatcher([](auto d) {
            d->Getter("name", &NodeDefinition::Name);
            d->Getter("inputs", [](NodeDefinition *p) { return &p->Inputs; });
            d->Getter("outputs", [](NodeDefinition *p) { return &p->Outputs; });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "node_definition");

    static perilune::UserType<NodeManager *> nodeManager;
    nodeManager
        .StaticMethod("new", []() { return new NodeManager; })
        .MetaMethod(perilune::MetaKey::__gc, [](NodeManager *p) { delete p; })
        .IndexDispatcher([](auto d) {
            d->Method("add_node", &NodeManager::AddNode);
            d->Method("get_count", &NodeManager::GetCount);
            d->Method("get_node", &NodeManager::GetNode);
            d->IndexGetter([](NodeManager *l, int i) {
                return l->GetNode(i - 1);
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "node_manager");

    lua_setglobal(L, "plugnode");
    assert(top == lua_gettop(L));
}
