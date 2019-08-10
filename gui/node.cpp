#include "node.h"
#include "context.h"
#include "nodedefinition.h"
#include "nodeslot.h"
#include <sstream>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
const float NODE_SLOT_RADIUS = 4.0f;

static int g_nodeId = 1;

const ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
const ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);

namespace plugnode
{

Node::Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos)
    : m_id(g_nodeId++), m_definition(definition), m_pos(pos)
{
    for (auto &in : definition->Inputs)
    {
        auto p = NodeSlot::CreateIn(in);
        m_inslots.push_back(p);
    }
    for (auto &out : definition->Outputs)
    {
        auto p = NodeSlot::CreateOut(out);
        m_outslots.push_back(p);
    }
}

const std::string &Node::GetName() const
{
    return m_definition->Name;
}

void Node::DrawLeftPanel(Context *context) const
{
    ImGui::PushID(m_id);
    if (ImGui::Selectable(m_definition->Name.c_str(), context->IsSelected(m_id)))
    {
        context->Select(m_id);
    }
    if (ImGui::IsItemHovered())
    {
        context->HoverInList(m_id);
    }
    ImGui::PopID();
}

std::array<float, 2> Node::GetInputSlotPos(int slot_no, float scaling) const
{
    auto x = m_pos[0] * scaling;
    auto y = m_pos[1] * scaling + m_size[1] * ((float)slot_no + 1) / ((float)m_definition->Inputs.size() + 1);
    return std::array<float, 2>{x, y};
}

std::array<float, 2> Node::GetOutputSlotPos(int slot_no) const
{
    return m_outslots[slot_no]->GetLinkPosition();
}

void Node::Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, float scaling)
{
    ImGui::PushID(m_id);

    // node左上
    ImVec2 node_rect_min = offset + *(ImVec2 *)&m_pos * scaling;

    //
    // Display node contents first
    //
    draw_list->ChannelsSetCurrent(1); // Foreground
    bool old_any_active = ImGui::IsAnyItemActive();
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

    {
        ImGui::BeginGroup(); // Lock horizontal position
        ImGui::Text("%s", m_definition->Name.c_str());
        for (auto &out : m_outslots)
        {
            out->ImGui();
        }
        ImGui::EndGroup();
    }

    // Save the size of what we have emitted and whether any of the widgets are being used
    bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
    auto size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
    m_size[0] = size.x;
    m_size[1] = size.y;
    ImVec2 node_rect_max = node_rect_min + size;

    //
    // Display node box
    //
    draw_list->ChannelsSetCurrent(0); // Background
    ImGui::SetCursorScreenPos(node_rect_min);
    ImGui::InvisibleButton("node", size);
    {
        // process input
        if (ImGui::IsItemHovered())
        {
            context->HoverInScene(m_id);
        }
        bool node_moving_active = ImGui::IsItemActive();
        if (node_widgets_active || node_moving_active)
        {
            context->Select(m_id);
        }
        if (node_moving_active && ImGui::IsMouseDragging(0))
        {
            m_pos[0] += ImGui::GetIO().MouseDelta[0] / scaling;
            m_pos[1] += ImGui::GetIO().MouseDelta[1] / scaling;
        }
        // draw
        draw_list->AddRectFilled(node_rect_min, node_rect_max, context->GetBGColor(m_id), 4.0f);
        draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
    }

    // slots
    for (int slot_idx = 0; slot_idx < m_definition->Inputs.size(); slot_idx++)
    {
        auto pos = *(ImVec2 *)&GetInputSlotPos(slot_idx, scaling);
        draw_list->AddCircleFilled(offset + pos, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
    }
    // for (int slot_idx = 0; slot_idx < m_definition->Outputs.size(); slot_idx++)
    // {
    //     draw_list->AddCircleFilled(offset + GetOutputSlotPos(slot_idx, scaling), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
    // }

    {
        for (auto &slot : m_outslots)
        {
            auto pos = slot->GetLinkPosition();
            draw_list->AddCircleFilled(ImVec2(pos[0] + NODE_WINDOW_PADDING.x, pos[1]), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
        }
    }

    ImGui::PopID();
}

} // namespace plugnode