#pragma once

namespace plugnode
{

struct Context
{
    bool open_context_menu = false;
    int node_hovered_in_list = -1;
    int node_hovered_in_scene = -1;
    int node_selected = -1;

    ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
    ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);

    bool IsHovered(int ID) const
    {
        return node_hovered_in_list == ID || node_hovered_in_scene == ID;
    }
    bool IsSelected(int ID) const
    {
        return node_hovered_in_list == -1 && node_selected == ID;
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
};

} // namespace plugnode