#include "node.h"
#include "context.h"
#include "nodedefinition.h"

const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
const float NODE_SLOT_RADIUS = 4.0f;

static int g_nodeId = 1;

namespace plugnode
{
Node::Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos)
    : m_id(g_nodeId++), m_definition(definition), m_pos(pos)
{
}

ImColor Node::GetBGColor(const Context &context, int node_selected) const
{
    if (context.IsHovered(m_id) || (context.node_hovered_in_list == -1 && node_selected == m_id))
    {
        return IM_COL32(75, 75, 75, 255);
    }
    else
    {
        return IM_COL32(60, 60, 60, 255);
    }
}

void Node::DrawLeftPanel(int *node_selected, Context *context) const
{
    ImGui::PushID(m_id);
    if (ImGui::Selectable(m_definition->Name.c_str(), m_id == *node_selected))
    {
        *node_selected = m_id;
    }
    if (ImGui::IsItemHovered())
    {
        context->node_hovered_in_list = m_id;
        (context->open_context_menu) |= ImGui::IsMouseClicked(1);
    }
    ImGui::PopID();
}

ImVec2 Node::GetInputSlotPos(int slot_no, float scaling) const
{
    auto x = m_pos[0] * scaling;
    auto y = m_pos[1] * scaling + m_size[1] * ((float)slot_no + 1) / ((float)m_definition->Inputs.size() + 1);
    return ImVec2(x, y);
}

ImVec2 Node::GetOutputSlotPos(int slot_no, float scaling) const
{
    auto x = m_pos[0] * scaling + m_size[0];
    auto y = m_pos[1] * scaling + m_size[1] * ((float)slot_no + 1) / ((float)m_definition->Outputs.size() + 1);
    return ImVec2(x, y);
}

void Node::Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, int *node_selected, float scaling)
{
    // Node *node = &nodes[node_idx];
    ImGui::PushID(m_id);
    ImVec2 node_rect_min = offset + *(ImVec2 *)&m_pos * scaling;

    // Display node contents first
    draw_list->ChannelsSetCurrent(1); // Foreground
    bool old_any_active = ImGui::IsAnyItemActive();
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
    ImGui::BeginGroup(); // Lock horizontal position
    ImGui::Text("%s", m_definition->Name.c_str());
    // ImGui::SliderFloat("##value", &Value, 0.0f, 1.0f, "Alpha %.2f");
    // ImGui::ColorEdit3("##color", &Color.x);
    ImGui::EndGroup();

    // Save the size of what we have emitted and whether any of the widgets are being used
    bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
    auto size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
    m_size[0] = size.x;
    m_size[1] = size.y;
    ImVec2 node_rect_max = node_rect_min + size;

    // Display node box
    draw_list->ChannelsSetCurrent(0); // Background
    ImGui::SetCursorScreenPos(node_rect_min);
    ImGui::InvisibleButton("node", size);
    if (ImGui::IsItemHovered())
    {
        context->node_hovered_in_scene = m_id;
        context->open_context_menu |= ImGui::IsMouseClicked(1);
    }
    bool node_moving_active = ImGui::IsItemActive();
    if (node_widgets_active || node_moving_active)
        *node_selected = m_id;
    if (node_moving_active && ImGui::IsMouseDragging(0))
    {
        m_pos[0] += ImGui::GetIO().MouseDelta[0] / scaling;
        m_pos[1] += ImGui::GetIO().MouseDelta[1] / scaling;
    }

    ImU32 node_bg_color = GetBGColor(*context, *node_selected);
    draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
    for (int slot_idx = 0; slot_idx < m_definition->Inputs.size(); slot_idx++)
    {
        draw_list->AddCircleFilled(offset + GetInputSlotPos(slot_idx, scaling), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
    }
    for (int slot_idx = 0; slot_idx < m_definition->Outputs.size(); slot_idx++)
    {
        draw_list->AddCircleFilled(offset + GetOutputSlotPos(slot_idx, scaling), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
    }

    ImGui::PopID();
}

NodeScene::NodeScene()
{
}

NodeScene::~NodeScene()
{
}

std::shared_ptr<Node> NodeScene::CreateNode(const std::shared_ptr<NodeDefinition> &definition, float x, float y)
{
    auto node = std::make_shared<Node>(definition, std::array<float, 2>{x, y});
    m_nodes.push_back(node);
    return node;
}

int NodeScene::GetIndex(const std::shared_ptr<Node> &node) const
{
    for (int i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i] == node)
        {
            return i;
        }
    }
    return -1;
}

std::shared_ptr<NodeLink> NodeScene::Link(const std::shared_ptr<Node> &src_node, int src_slot,
                                          const std::shared_ptr<Node> &dst_node, int dst_slot)
{
    auto src_index = GetIndex(src_node);
    if (src_index == -1)
    {
        return nullptr;
    }
    auto dst_index = GetIndex(dst_node);
    if (dst_index == -1)
    {
        return nullptr;
    }
    auto link = std::make_shared<NodeLink>(src_index, src_slot, dst_index, dst_slot);
    m_links.push_back(link);
    return link;
}

} // namespace plugnode