#include "nodegraph.h"
#include "context.h"
#include "node.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <plog/Log.h>
#include <vector>
#include <perilune/perilune.h>

// Creating a node graph editor for ImGui
// Quick demo, not production code! This is more of a demo of how to use ImGui to create custom stuff.
// Better version by @daniel_collin here https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// See https://github.com/ocornut/imgui/issues/306
// v0.03: fixed grid offset issue, inverted sign of 'scrolling'
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9472357/c0263c04-4b4c-11e5-9fdf-2cd4f33f6582.gif

#include <math.h> // fmodf

const float MIN_SCALING = 0.3f;
const float MAX_SCALING = 2.0f;

namespace plugnode
{

class NodeGraphImpl
{
    ImVec2 m_scrolling = ImVec2(0.0f, 0.0f);
    float m_scaling = 1.0f;
    bool m_show_grid = true;
    int m_node_selected = -1;

    bool m_show = true;

public:
    NodeGraphImpl()
    {
    }

    void Show(const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        // ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Example: Custom Node Graph", &m_show))
        {
            Context context;

            ShowLeftPanel(&context, definitions, scene);

            ImGui::SameLine();
            ImGui::BeginGroup();
            ShowRightPanel(&context, definitions, scene);
            ImGui::EndGroup();
        }
        ImGui::End();
    }

private:
    void ShowLeftPanel(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        // Draw a list of nodes on the left side
        ImGui::BeginChild("node_list", ImVec2(100, 0));
        ImGui::Text("Nodes");
        ImGui::Separator();

        for (auto &node : scene->m_nodes)
        {
            node->DrawLeftPanel(&m_node_selected, context);
        }
        ImGui::EndChild();
    }

    void DrawGrid(ImDrawList *draw_list, const ImVec2 &_scrolling)
    {
        ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
        float GRID_SZ = 64.0f * m_scaling;
        ImVec2 win_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImGui::GetWindowSize();
        auto scrolling = _scrolling;
        for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
        }
        for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
        }
    }

    void ContextMenu(Context *context, const ImVec2 &offset,
                     const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
        {
            m_node_selected = context->node_hovered_in_list = context->node_hovered_in_scene = -1;
            context->open_context_menu = true;
        }
        if (context->open_context_menu)
        {
            ImGui::OpenPopup("context_menu");
            if (context->node_hovered_in_list != -1)
                m_node_selected = context->node_hovered_in_list;
            if (context->node_hovered_in_scene != -1)
                m_node_selected = context->node_hovered_in_scene;
        }

        // Draw context menu
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginPopup("context_menu"))
        {
            ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
            if (m_node_selected != -1)
            {
                auto &node = scene->m_nodes[m_node_selected];
                ImGui::Text("Node '%s'", node->m_definition->Name.c_str());
                ImGui::Separator();
                if (ImGui::MenuItem("Rename..", NULL, false, false))
                {
                }
                if (ImGui::MenuItem("Delete", NULL, false, false))
                {
                }
                if (ImGui::MenuItem("Copy", NULL, false, false))
                {
                }
            }
            else
            {
                for (auto p : definitions->m_definitions)
                {
                    if (ImGui::MenuItem(p->Name.c_str()))
                    {
                        scene->CreateNode(p, scene_pos[0], scene_pos[1]);
                    }
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    void ShowRightPanelHeader()
    {
        // right header
        ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", m_scrolling.x, m_scrolling.y);

        ImGui::SameLine();
        ImGui::Checkbox("Show grid", &m_show_grid);

        ImGui::SameLine();
        ImGui::PushItemWidth(-100);
        ImGui::SliderFloat("zoom", &m_scaling, MIN_SCALING, MAX_SCALING, "%0.2f");
    }

    ///
    /// Canvas
    ///
    void ShowRightPanelCanvas(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        // スクロールを加味したcanvasの原点
        ImVec2 offset = ImGui::GetCursorScreenPos() + m_scrolling;

        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        // Display grid
        if (m_show_grid)
        {
            DrawGrid(draw_list, m_scrolling);
        }

        {
            draw_list->ChannelsSplit(2);
            draw_list->ChannelsSetCurrent(0); // Background
            // Display links
            for (auto &link : scene->m_links)
            {
                auto &node_inp = scene->m_nodes[link->InputIdx];
                auto &node_out = scene->m_nodes[link->OutputIdx];
                ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot, m_scaling);
                ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot, m_scaling);
                draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f * m_scaling);
            }

            // Display nodes
            for (auto &node : scene->m_nodes)
            {
                // move, draw
                node->Process(draw_list, offset, context, &m_node_selected, m_scaling);
            }
            draw_list->ChannelsMerge();
        }

        // Open context menu
        ContextMenu(context, offset, definitions, scene);

        // Scrolling
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
        {
            if (ImGui::IsMouseDragging(2, 0.0f))
            {
                m_scrolling = m_scrolling + ImGui::GetIO().MouseDelta;
            }
            auto io = ImGui::GetIO();
            if (io.MouseWheel > 0)
            {
                //m_scaling *= 1.25f;
                m_scaling += 0.1f;
            }
            else if (io.MouseWheel < 0)
            {
                //m_scaling *= 0.8f;
                m_scaling -= 0.1f;
            }
            m_scaling = std::clamp(m_scaling, MIN_SCALING, MAX_SCALING);
        }
    }

    void ShowRightPanel(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        ShowRightPanelHeader();

        // Create our child canvas
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
        ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(120.0f * m_scaling);

        auto backup = ImGui::GetStyle();
        ImGui::GetStyle().ScaleAllSizes(m_scaling);
        ImGui::SetWindowFontScale(m_scaling);
        ShowRightPanelCanvas(context, definitions, scene);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::GetStyle() = backup;

        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
};

NodeGraph::NodeGraph()
    : m_impl(new NodeGraphImpl)
{
}

NodeGraph::~NodeGraph()
{
    delete m_impl;
}

void NodeGraph::ImGui(NodeDefinitionManager *definitions,
                      NodeScene *scene)
{
    m_impl->Show(definitions, scene);
}

void lua_require_plugnode(lua_State *L)
{
    auto top = lua_gettop(L);
    lua_newtable(L);

#pragma region definitions
    static perilune::UserType<plugnode::NodeSocket> nodeSocket;
    nodeSocket
        .StaticMethod("new", [](std::string name, std::string type) {
            return plugnode::NodeSocket{name, type};
        })
        .MetaMethod(perilune::MetaKey::__tostring, [](plugnode::NodeSocket *socket) {
            std::stringstream ss;
            ss << "[" << socket->type << "]" << socket->name;
            return ss.str();
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "node_socket");

    static perilune::UserType<std::vector<plugnode::NodeSocket> *>
        stringList;
    perilune::AddDefaultMethods(stringList);
    stringList
        .LuaNewType(L);
    lua_setfield(L, -2, "string_list");

    static perilune::UserType<std::shared_ptr<plugnode::NodeDefinition>> nodeDefinition;
    nodeDefinition
        .MetaIndexDispatcher([](auto d) {
            d->Getter("name", &plugnode::NodeDefinition::Name);
            d->Getter("inputs", [](plugnode::NodeDefinition *p) { return &p->Inputs; });
            d->Getter("outputs", [](plugnode::NodeDefinition *p) { return &p->Outputs; });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "definition");

    static perilune::UserType<plugnode::NodeDefinitionManager *> definitions;
    definitions
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", [](plugnode::NodeDefinitionManager *p, std::string name) {
                return p->Create(name);
            });
            d->Method("get_count", &plugnode::NodeDefinitionManager::GetCount);
            d->IndexGetter([](plugnode::NodeDefinitionManager *l, int i) {
                return l->Get(i - 1);
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "definition_manager");
#pragma endregion

#pragma region scene
    static perilune::UserType<std::shared_ptr<plugnode::Node>> node;
    node
        .LuaNewType(L);
    lua_setfield(L, -2, "node");

    static perilune::UserType<std::shared_ptr<plugnode::NodeLink>> link;
    link
        .LuaNewType(L);
    lua_setfield(L, -2, "link");

    static perilune::UserType<plugnode::NodeScene *> nodescene;
    nodescene
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", [](plugnode::NodeScene *p, const std::shared_ptr<NodeDefinition> &def, float x, float y) {
                return p->CreateNode(def, x, y);
            });
            d->Method("link", [](plugnode::NodeScene *p,
                                 const std::shared_ptr<plugnode::Node> &src, int src_slot,
                                 const std::shared_ptr<plugnode::Node> &dst, int dst_slot) {
                return p->Link(src, src_slot - 1, dst, dst_slot - 1);
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "scene");
#pragma endregion

    static perilune::UserType<plugnode::NodeGraph *> nodegraph;
    nodegraph
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("imgui", &plugnode::NodeGraph::ImGui);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "graph");

    lua_setglobal(L, "plugnode");
    assert(top == lua_gettop(L));
}

} // namespace plugnode

namespace perilune
{

template <>
struct LuaTable<plugnode::NodeSocket>
{
    static plugnode::NodeSocket Get(lua_State *L, int index)
    {
        auto table = LuaTableToTuple<std::string, std::string>(L, index);
        return plugnode::NodeSocket{
            std::get<0>(table),
            std::get<1>(table),
        };
    }
};

} // namespace perilune
