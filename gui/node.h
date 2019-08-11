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

class NodeSlot;
class Context;
class NodeDefinition;
class Node
{
    int m_id;
    std::shared_ptr<NodeDefinition> m_definition;
    std::array<float, 2> m_pos;
    std::array<float, 2> m_size;

public:
    struct InSlot
    {
        InSlot(const InSlot &) = delete;
        InSlot &operator=(const InSlot &) = delete;

        InSlot(const std::shared_ptr<NodeSlot> &slot)
            : Slot(slot)
        {
        }

        InSlot(InSlot &&src)
        {
            Slot = std::move(src.Slot);
            Src = std::move(src.Src);
        }

        InSlot();

        std::shared_ptr<NodeSlot> Slot;
        std::weak_ptr<NodeSlot> Src;

        void Link(const std::shared_ptr<NodeSlot> &src)
        {
            Src = src;
        }
    };
    std::vector<InSlot> m_inslots;
    std::vector<std::shared_ptr<NodeSlot>> m_outslots;

    Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos);
    int GetId() const { return m_id; }
    const std::string &GetName() const;

    void DrawLeftPanel(Context *context) const;

    std::array<float, 2> GetInputSlotPos(int slot_no) const;
    std::array<float, 2> GetOutputSlotPos(int slot_no) const;

    void Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, float scaling);
};

} // namespace plugnode