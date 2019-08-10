#pragma once
#include <imgui.h>

namespace plugnode
{

class Context
{
    bool m_open_context_menu = false;
    int m_node_hovered_in_list = -1;
    int m_node_hovered_in_scene = -1;
    int m_node_selected = -1;

public:
    ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
    ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);


    void NewFrame()
    {
        m_open_context_menu = false;
        m_node_hovered_in_list = -1;
        m_node_hovered_in_scene = -1;
    }

    void Select(int id)
    {
        m_node_selected = id;
    }

    void HoverInList(int id)
    {
        m_node_hovered_in_list = id;
        if (ImGui::IsMouseClicked(1))
        {
            // right click
            m_open_context_menu = true;
        }
    }

    void HoverInScene(int id)
    {
        m_node_hovered_in_scene = id;
        if (ImGui::IsMouseClicked(1))
        {
            // right click
            m_open_context_menu = true;
        }
    }

    bool IsHovered(int ID) const
    {
        return m_node_hovered_in_list == ID || m_node_hovered_in_scene == ID;
    }

    bool IsSelected(int ID) const
    {
        return m_node_hovered_in_list == -1 && m_node_selected == ID;
    }

    ImU32 GetBGColor(int m_id) const
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

    void ContextMenu(const ImVec2 &offset,
                     const class NodeDefinitionManager *definitions,
                     class NodeScene *scene);
};

} // namespace plugnode