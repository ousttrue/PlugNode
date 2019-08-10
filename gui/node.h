#pragma once
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <any>

namespace plugnode
{

class NodeSlot;
class Context;
class NodeDefinition;
struct Node
{
    int m_id;
    std::shared_ptr<NodeDefinition> m_definition;
    std::array<float, 2> m_pos;
    std::array<float, 2> m_size;

    ImVec2 m_slotorigin;
    std::vector<std::shared_ptr<NodeSlot>> m_outslots;

    Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos);

    void DrawLeftPanel(Context *context) const;

    ImVec2 GetInputSlotPos(int slot_no, float scaling) const;
    ImVec2 GetOutputSlotPos(int slot_no, float scaling) const;

    void Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, float scaling);
};

struct NodeLink
{
    int InputIdx;
    int InputSlot;
    int OutputIdx;
    int OutputSlot;

    NodeLink(int input_idx, int input_slot, int output_idx, int output_slot)
    {
        InputIdx = input_idx;
        InputSlot = input_slot;
        OutputIdx = output_idx;
        OutputSlot = output_slot;
    }
};


} // namespace plugnode