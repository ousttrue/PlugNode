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

    std::vector<std::shared_ptr<NodeSlot>> m_outslots;

public:
    Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos);
    int GetId() const { return m_id; }
    const std::string &GetName() const;

    void DrawLeftPanel(Context *context) const;

    std::array<float, 2> GetInputSlotPos(int slot_no, float scaling) const;
    std::array<float, 2> GetOutputSlotPos(int slot_no, float scaling) const;

    void Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, float scaling);
};

struct NodeLink
{
    int SrcNode;
    int SrcSlot;
    int DstNode;
    int DstSlot;

    NodeLink(int input_idx, int input_slot, int output_idx, int output_slot)
    {
        SrcNode = input_idx;
        SrcSlot = input_slot;
        DstNode = output_idx;
        DstSlot = output_slot;
    }
};

} // namespace plugnode