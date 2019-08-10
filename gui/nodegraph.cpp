#include "nodegraph.h"
#include "context.h"
#include "node.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <plog/Log.h>
#include <vector>

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
    bool m_show = true;

    Context m_context;

public:
    NodeGraphImpl()
    {
    }

    void Show(const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        // ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Example: Custom Node Graph", &m_show))
        {
            m_context.NewFrame();

            ShowLeftPanel(&m_context, definitions, scene);

            ImGui::SameLine();
            ImGui::BeginGroup();
            ShowRightPanel(&m_context, definitions, scene);
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
            node->DrawLeftPanel(context);
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
                node->Process(draw_list, offset, context, m_scaling);
            }
            draw_list->ChannelsMerge();
        }

        // Open context menu
        context->ContextMenu(offset, definitions, scene);

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

} // namespace plugnode
