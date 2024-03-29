#pragma once
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <any>

struct ImDrawList;
struct ImVec2;
namespace plugnode
{

class Context;
class NodeDefinition;
class InSlotBase;
class OutSlotBase;
class Node
{
    int m_id;
    // std::shared_ptr<NodeDefinition> m_definition;
    std::array<float, 2> m_pos;
    std::array<float, 2> m_size;

public:
    std::vector<std::shared_ptr<InSlotBase>> m_inslots;
    std::vector<std::shared_ptr<OutSlotBase>> m_outslots;

    Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos);
    int GetId() const { return m_id; }
    std::array<char, 64> Name;
    std::shared_ptr<NodeDefinition> Definition;

    void DrawLeftPanel(Context *context) const;

    bool Process(ImDrawList *draw_list, Context *context);

private:
    bool _DrawSlots(ImDrawList *draw_list, const ImVec2 &node_rect_min, Context *context);
};

} // namespace plugnode