﻿#include "node.h"
#include "context.h"
#include "nodedefinition.h"
#include "nodeslot/nodeslot.h"
#include <sstream>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

static int g_nodeId = 1;

const ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
const ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);

namespace plugnode
{

Node::Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos)
    : m_id(g_nodeId++), Definition(definition), m_pos(pos)
{
    int i = 0;
    for (; i < definition->DefaultName.size(); ++i)
    {
        Name[i] = definition->DefaultName[i];
    }
    Name[i] = 0;

    if (!definition->Inputs.empty())
    {
        if (!definition->Outputs.empty())
        {
            // both
            for (auto &in : definition->Inputs)
            {
                auto p = InSlotBase::Create(in, SlotType::LabelOnly);
                m_inslots.push_back(p);
            }
            for (auto &out : definition->Outputs)
            {
                auto p = OutSlotBase::Create(out, SlotType::Value);
                m_outslots.push_back(p);
            }
        }
        else
        {
            // only input
            for (auto &in : definition->Inputs)
            {
                auto p = InSlotBase::Create(in, SlotType::Value);
                m_inslots.push_back({p});
            }
        }
    }
    else
    {
        if (!definition->Outputs.empty())
        {
            // only output
            for (auto &out : definition->Outputs)
            {
                auto p = OutSlotBase::Create(out, SlotType::UserInterface);
                m_outslots.push_back(p);
            }
        }
        else
        {
            throw std::exception("no slots");
        }
    }
}

void Node::DrawLeftPanel(Context *context) const
{
    ImGui::PushID(m_id);
    if (ImGui::Selectable(Name.data(), context->IsSelected(m_id)))
    {
        context->Select(m_id);
    }
    if (ImGui::IsItemHovered())
    {
        context->HoverInList(m_id);
    }
    ImGui::PopID();
}

bool Node::Process(ImDrawList *draw_list, Context *context)
{
    ImGui::PushID(m_id);

    // node左上
    auto node_rect_min = *(ImVec2 *)&context->GetNodePosition(m_pos[0], m_pos[1]);

    bool old_any_active = ImGui::IsAnyItemActive();

    // Display node contents first
    // ImGui::PushItemWidth(80 * scaling);
    auto is_updated = _DrawSlots(draw_list, node_rect_min, context);
    // ImGui::PopItemWidth();

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

        // move
        if (node_moving_active && ImGui::IsMouseDragging(0))
        {
            auto scaling = context->GetScaling();
            m_pos[0] += ImGui::GetIO().MouseDelta[0] / scaling;
            m_pos[1] += ImGui::GetIO().MouseDelta[1] / scaling;
        }

        // draw
        draw_list->AddRectFilled(node_rect_min, node_rect_max, context->GetBGColor(m_id), 4.0f);
        draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
    }

    ImGui::PopID();

    return is_updated;
}

bool Node::_DrawSlots(ImDrawList *draw_list, const ImVec2 &node_rect_min, Context *context)
{
    draw_list->ChannelsSetCurrent(1); // Foreground
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

    auto is_updated = false;
    {
        ImGui::BeginGroup(); // Lock horizontal position
        ImGui::Text("%s", Definition->Name.c_str());
        ImGui::InputText("##name", Name.data(), Name.size());

        if (!m_inslots.empty())
        {
            if (!m_outslots.empty())
            {
                auto origin = ImGui::GetCursorScreenPos();
                for (auto &in : m_inslots)
                {
                    in->ImGui(draw_list, context);
                }

                ImGui::SetCursorScreenPos(ImVec2(origin.x + m_inslots[0]->Rect[2] + 8, origin.y));
                ImGui::BeginGroup(); // Lock horizontal position
                for (auto &out : m_outslots)
                {
                    out->ImGui(draw_list, context);
                }
                ImGui::EndGroup();
            }
            else
            {
                for (auto &in : m_inslots)
                {
                    in->ImGui(draw_list, context);
                }
            }
        }
        else
        {
            if (!m_outslots.empty())
            {
                for (auto &out : m_outslots)
                {
                    if (out->ImGui(draw_list, context))
                    {
                        is_updated = true;
                    }
                }
            }
            else
            {
                throw std::exception("no slots");
            }
        }

        ImGui::EndGroup();
    }
    return is_updated;
}

} // namespace plugnode