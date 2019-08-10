#pragma once
#include <stdint.h>

struct ImVec2;
namespace plugnode
{

class Context
{
    bool m_open_context_menu = false;
    int m_node_hovered_in_list = -1;
    int m_node_hovered_in_scene = -1;
    int m_node_selected = -1;

public:
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

    void HoverInList(int id);
    void HoverInScene(int id);
    bool IsHovered(int ID) const;
    bool IsSelected(int ID) const;
    uint32_t GetBGColor(int m_id) const;
    void ContextMenu(const ImVec2 &offset,
                     const class NodeDefinitionManager *definitions,
                     class NodeScene *scene);
};

} // namespace plugnode