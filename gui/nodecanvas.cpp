#include "nodecanvas.h"
#include "context.h"
#include "node.h"
#include "nodescene.h"

#include <math.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

const float MIN_SCALING = 0.3f;
const float MAX_SCALING = 2.0f;
const ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);

namespace plugnode
{

class NodeCanvasImpl
{
    float m_scaling = 1.0f;
    ImVec2 m_scrolling = ImVec2(0.0f, 0.0f);
    bool m_show_grid = true;

public:
    void Show(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        ShowHeader();

        // Create our child canvas
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
        {
            ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
            ImGui::PushItemWidth(120.0f * m_scaling);

            auto backup = ImGui::GetStyle();
            ImGui::GetStyle().ScaleAllSizes(m_scaling);
            ImGui::SetWindowFontScale(m_scaling);
            ShowCanvas(context, definitions, scene);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::GetStyle() = backup;

            ImGui::PopItemWidth();
            ImGui::EndChild();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

private:
    void ShowHeader()
    {
        ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", m_scrolling.x, m_scrolling.y);

        ImGui::SameLine();
        ImGui::Checkbox("Show grid", &m_show_grid);

        ImGui::SameLine();
        ImGui::PushItemWidth(-100);
        ImGui::SliderFloat("zoom", &m_scaling, MIN_SCALING, MAX_SCALING, "%0.2f");
    }

    void DrawGrid(ImDrawList *draw_list, const ImVec2 &scrolling)
    {
        float GRID_SZ = 64.0f * m_scaling;
        ImVec2 win_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImGui::GetWindowSize();
        for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
        }
        for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
        }
    }

    ///
    /// Canvas
    ///
    void ShowCanvas(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
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
            // 0: Node contents
            // 1: Others(node rect, link curve...)

            // Display links
            draw_list->ChannelsSetCurrent(0);
            for (auto &link : scene->m_links)
            {
                auto p1 = *(ImVec2 *)&scene->GetLinkSrc(link);

                auto &dst = scene->GetFromId(link->DstNode);
                ImVec2 p2 = offset + *(ImVec2 *)&dst->GetInputSlotPos(link->DstSlot, m_scaling);

                draw_list->AddBezierCurve(
                    p1,
                    p1 + ImVec2(+50, 0),
                    p2 + ImVec2(-50, 0),
                    p2,
                    IM_COL32(200, 200, 100, 255), 3.0f * m_scaling);
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

        // Zoom and Scroll
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
        {
            if (ImGui::IsMouseDragging(2, 0.0f))
            {
                m_scrolling = m_scrolling + ImGui::GetIO().MouseDelta;
            }
            auto io = ImGui::GetIO();
            if (io.MouseWheel > 0)
            {
                m_scaling += 0.1f;
            }
            else if (io.MouseWheel < 0)
            {
                m_scaling -= 0.1f;
            }
            m_scaling = std::clamp(m_scaling, MIN_SCALING, MAX_SCALING);
        }
    }
};

NodeCanvas::NodeCanvas()
    : m_impl(new NodeCanvasImpl)
{
}

NodeCanvas::~NodeCanvas()
{
    delete m_impl;
}

void NodeCanvas::Show(Context *context,
                      const NodeDefinitionManager *definitions,
                      NodeScene *scene)
{
    m_impl->Show(context, definitions, scene);
}

} // namespace plugnode