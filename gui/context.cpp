#include "context.h"
#include "nodedefinition.h"
#include "node.h"
#include "nodescene.h"
#include "nodeslot/nodeslot.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

const ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
const ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
const ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);
const ImU32 PIN_COLOR = IM_COL32(150, 150, 150, 150);
const ImU32 PIN_HOVER_COLOR = IM_COL32(150, 250, 150, 150);
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

const float MIN_SCALING = 0.3f;
const float MAX_SCALING = 2.0f;
const float LINK_WIDTH = 3.0f;
const float NODE_SLOT_RADIUS = 6.0f;

namespace plugnode
{

static float Dot(const ImVec2 &v)
{
    return v.x * v.x + v.y * v.y;
}

class ContextImpl
{
#pragma region Canvas
    ImVec2 m_scrolling = ImVec2(0.0f, 0.0f);
    bool m_show_grid = true;
    float m_scaling = 1.0f;

    ImGuiStyle m_backup;
    ImVec2 m_canvas_position;
#pragma endregion

public:
    void BeginScaling()
    {
        m_backup = ImGui::GetStyle();
        ImGui::GetStyle().ScaleAllSizes(m_scaling);
        ImGui::SetWindowFontScale(m_scaling);
        ImGui::PushItemWidth(100 * m_scaling);
    }
    void EndScaling()
    {
        ImGui::PopItemWidth();
        ImGui::SetWindowFontScale(1.0f);
        ImGui::GetStyle() = m_backup;
    }

    void ShowHeader()
    {
        ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", m_scrolling.x, m_scrolling.y);

        ImGui::SameLine();
        ImGui::Checkbox("Show grid", &m_show_grid);

        ImGui::SameLine();
        ImGui::PushItemWidth(-100);
        ImGui::SliderFloat("zoom", &m_scaling, MIN_SCALING, MAX_SCALING, "%0.2f");
    }

    float GetScaling() const
    {
        return m_scaling;
    }

    void DrawGrid(ImDrawList *draw_list)
    {
        if (!m_show_grid)
        {
            return;
        }

        float GRID_SZ = 64.0f * m_scaling;
        ImVec2 win_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImGui::GetWindowSize();
        for (float x = fmodf(m_scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
        }
        for (float y = fmodf(m_scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
        {
            draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
        }
    }

    void BeginCanvas(ImDrawList *draw_list)
    {
        BeginScaling();

        m_canvas_position = ImGui::GetCursorScreenPos();
        DrawGrid(draw_list);
    }

    void EndCanvas()
    {
        // Zoom and Scroll
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

            if (ImGui::IsMouseDragging(2, 0.0f))
            {
                m_scrolling = m_scrolling + ImGui::GetIO().MouseDelta;
            }

            auto mouse = ImGui::GetIO().MousePos - m_canvas_position;
            ImVec2 focus = (mouse - m_scrolling) / m_scaling;

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

            auto new_mouse = m_scrolling + (focus * m_scaling);
            m_scrolling += (mouse - new_mouse);
        }

        EndScaling();
    }

    ImVec2 GetScrollingPosition() const
    {
        return m_canvas_position + m_scrolling;
    }

    float GetLinkWidth() const
    {
        return LINK_WIDTH * m_scaling;
    }

    ImVec2 GetNodePosition(const ImVec2 &pos) const
    {
        return GetScrollingPosition() + pos * m_scaling;
    }

    bool DrawPin(ImDrawList *draw_list, const ImVec2 &pos)
    {
        // auto pos = *(ImVec2 *)&GetPin()->Position;
        auto mouse = ImGui::GetMousePos();
        auto dot = Dot(pos - mouse);
        auto r = NODE_SLOT_RADIUS * m_scaling;
        auto isHover = dot <= r * r;
        if (isHover)
        {
            // on mouse
            draw_list->AddCircleFilled(pos, r, PIN_HOVER_COLOR);
        }
        else
        {
            draw_list->AddCircleFilled(pos, r, PIN_COLOR);
        }
        return isHover;
    }
};

bool Context::s_popup = false;

Context::Context()
    : m_impl(new ContextImpl)
{
}

Context::~Context()
{
    delete m_impl;
}

void Context::HoverInList(int id)
{
    m_node_hovered_in_list = id;
    if (ImGui::IsMouseClicked(1))
    {
        // right click
        m_open_context_menu = true;
    }
}

void Context::HoverInScene(int id)
{
    m_node_hovered_in_scene = id;
    if (ImGui::IsMouseClicked(1))
    {
        // right click
        m_open_context_menu = true;
    }
}

bool Context::IsHovered(int ID) const
{
    return m_node_hovered_in_list == ID || m_node_hovered_in_scene == ID;
}

bool Context::IsSelected(int ID) const
{
    return m_node_hovered_in_list == -1 && m_node_selected == ID;
}

ImU32 Context::GetBGColor(int m_id) const
{
    if (IsHovered(m_id) || IsSelected(m_id))
    {
        return NODE_HOVER_COLOR;
    }
    else
    {
        return NODE_COLOR;
    }
}

bool Context::ProcessClick(
    const NodeDefinitionManager *definitions,
    NodeScene *scene)
{
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(1))
    {
        // right click on canvas
        m_node_selected = m_node_hovered_in_list = m_node_hovered_in_scene = -1;
        m_open_context_menu = true;
        m_activeSlot = nullptr;
    }

    bool isUpdated = false;
    if (ImGui::IsMouseClicked(0))
    {
        if (m_activeSlot)
        {
            auto slot = scene->GetHoverInSlot();
            if (slot)
            {
                if (slot->Link(m_activeSlot))
                {
                    //
                    isUpdated = true;
                }
            }
            m_activeSlot = nullptr;
        }
        else
        {
            auto outSlot = scene->GetHoverOutSlot();
            if (outSlot)
            {
                m_activeSlot = outSlot;
            }
            else
            {
                auto inSlot = scene->GetHoverInSlot();
                if (inSlot)
                {
                    if (inSlot->Disconnect())
                    {
                        isUpdated = true;
                    }
                }
            }
        }
    }

    if (m_open_context_menu)
    {
        ImGui::OpenPopup("context_menu");
        if (m_node_hovered_in_list != -1)
            m_node_selected = m_node_hovered_in_list;
        if (m_node_hovered_in_scene != -1)
            m_node_selected = m_node_hovered_in_scene;
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    s_popup = ImGui::BeginPopup("context_menu");
    if (s_popup)
    {
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - m_impl->GetScrollingPosition();
        if (m_node_selected != -1)
        {
            auto node = scene->GetFromId(m_node_selected);
            ImGui::Text("Node '%s'", node->Name.c_str());
            ImGui::Separator();
            /*
            if (ImGui::MenuItem("Rename..", NULL, false, false))
            {
            }
            */
            if (ImGui::MenuItem("Delete", NULL))
            {
                scene->Remove(node);
                isUpdated = true;
            }
            /*
            if (ImGui::MenuItem("Copy", NULL, false, false))
            {
            }
            */
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

    return isUpdated;
}

void Context::DrawLink(ImDrawList *draw_list)
{
    if (!m_activeSlot)
    {
        return;
    }
    auto p1 = *(ImVec2 *)&m_activeSlot->GetPin()->Position;
    auto p2 = ImGui::GetMousePos();

    draw_list->AddBezierCurve(
        p1,
        p1 + ImVec2(+50, 0),
        p2 + ImVec2(-50, 0),
        p2,
        IM_COL32(200, 200, 100, 255), m_impl->GetLinkWidth());
}

void Context::BeginCanvas(ImDrawList *draw_list)
{
    m_impl->BeginCanvas(draw_list);
    draw_list->ChannelsSplit(2);
    // 0: Node contents
    // 1: Others(node rect, link curve...)
}

void Context::EndCanvas(ImDrawList *draw_list)
{
    draw_list->ChannelsMerge();
    m_impl->EndCanvas();
}

float Context::GetScaling() const
{
    return m_impl->GetScaling();
}

void Context::ShowHeader()
{
    m_impl->ShowHeader();
}

std::array<float, 2> Context::GetNodePosition(float x, float y) const
{
    return *(std::array<float, 2> *)&m_impl->GetNodePosition(ImVec2(x, y));
}

float Context::GetLinkWidth() const
{
    return m_impl->GetLinkWidth();
}

bool Context::DrawPin(ImDrawList *draw_list, float x, float y)
{
    return m_impl->DrawPin(draw_list, ImVec2(x, y));
}

float Context::GetNodeHorizontalPadding() const
{
    return NODE_WINDOW_PADDING.x;
}

} // namespace plugnode