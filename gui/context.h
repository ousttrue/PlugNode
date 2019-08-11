#pragma once
#include <stdint.h>
#include <memory>

struct ImVec2;
struct ImDrawList;
namespace plugnode
{

class OutSlotBase;
class Context
{
    bool m_open_context_menu = false;
    int m_node_hovered_in_list = -1;
    int m_node_hovered_in_scene = -1;
    int m_node_selected = -1;
    std::shared_ptr<OutSlotBase> m_activeSlot;

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
    void ProcessClick(const ImVec2 &offset,
                     const class NodeDefinitionManager *definitions,
                     class NodeScene *scene);
    void DrawLink(ImDrawList *draw_list, float width);
};

} // namespace plugnode