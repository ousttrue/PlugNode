#pragma once
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <string>
#include <array>
#include <vector>
#include <memory>

namespace plugnode
{

struct Context;
struct Node
{
    int m_id;
    std::string m_name;
    std::array<float, 2> m_pos;
    std::array<float, 2> m_size;

    float Value;
    ImVec4 Color;
    int InputsCount, OutputsCount;

    Node(const std::string &name, const std::array<float, 2> &pos, float value, const ImVec4 &color, int inputs_count, int outputs_count);

    ImColor GetBGColor(const Context &context, int node_selected) const;

    void DrawLeftPanel(int *node_selected, Context *context) const;

    ImVec2 GetInputSlotPos(int slot_no, float scaling) const;
    ImVec2 GetOutputSlotPos(int slot_no, float scaling) const;

    void Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, int *node_selected, float scaling);
};

struct NodeLink
{
    int InputIdx, InputSlot, OutputIdx, OutputSlot;

    NodeLink(int input_idx, int input_slot, int output_idx, int output_slot)
    {
        InputIdx = input_idx;
        InputSlot = input_slot;
        OutputIdx = output_idx;
        OutputSlot = output_slot;
    }
};

class NodeDefinition;
class NodeScene
{
public:
    std::vector<std::unique_ptr<Node>> m_nodes;
    std::vector<std::unique_ptr<NodeLink>> m_links;

public:
    NodeScene();
    ~NodeScene();
    void CreateNode(const NodeDefinition *definition, float x, float y);
};

} // namespace plugnode