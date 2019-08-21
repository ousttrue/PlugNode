#include "nodegraph.h"
#include "context.h"
#include "node.h"
#include "nodescene.h"
#include "nodeslot/nodeslot.h"
#include <plog/Log.h>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// Creating a node graph editor for ImGui
// Quick demo, not production code! This is more of a demo of how to use ImGui to create custom stuff.
// Better version by @daniel_collin here https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// See https://github.com/ocornut/imgui/issues/306
// v0.03: fixed grid offset issue, inverted sign of 'scrolling'
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9472357/c0263c04-4b4c-11e5-9fdf-2cd4f33f6582.gif

#include <math.h> // fmodf

namespace plugnode
{

class NodeGraphImpl
{
    Context m_context;
    // NodeCanvas m_canvas;

public:
    NodeGraphImpl()
    {
    }

    bool Show(const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        m_context.NewFrame();

        {
            // 横幅を制限した描画
            ImGui::BeginChild("node_list", ImVec2(100, 0));
            ImGui::Text("Nodes");
            ImGui::Separator();

            for (auto &node : scene->m_nodes)
            {
                node->DrawLeftPanel(&m_context);
            }
            ImGui::EndChild();
        }

        bool isUpdated = false;

        {
            // 右隣に描画
            ImGui::SameLine();
            ImGui::BeginGroup();
            {
                m_context.ShowHeader();

                // Create our child canvas
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
                {
                    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
                    ImDrawList *draw_list = ImGui::GetWindowDrawList();
                    m_context.BeginCanvas(draw_list);
                    {
                        // Display links
                        draw_list->ChannelsSetCurrent(0);
                        for (auto &node : scene->m_nodes)
                        {
                            for (auto &inSlot : node->m_inslots)
                            {
                                inSlot->DrawLink(draw_list, &m_context);
                            }
                        }
                        m_context.DrawLink(draw_list);

                        // Display nodes
                        for (auto &node : scene->m_nodes)
                        {
                            // move, draw
                            if (node->Process(draw_list, &m_context))
                            {
                                isUpdated = true;
                            }
                        }
                        // Open context menu
                        if (m_context.ProcessClick(definitions, scene))
                        {
                            isUpdated = true;
                        }
                    }
                    m_context.EndCanvas(draw_list);
                    ImGui::EndChild();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);
            }
            ImGui::EndGroup();
        }

        return isUpdated;
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

bool NodeGraph::ImGui(NodeDefinitionManager *definitions,
                      NodeScene *scene)
{
    static bool s_show = true;
    static bool s_isFirst = true;
    bool isUpdated = false;
    if (s_isFirst)
    {
        s_isFirst = false;
        isUpdated = true;
    }

    if (ImGui::Begin("PlugNode", &s_show))
    {
        if (m_impl->Show(definitions, scene))
        {
            isUpdated = true;
        }
    }
    ImGui::End();
    return isUpdated;
}

} // namespace plugnode
