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
class NodeDefinition;
struct Node
{
    int m_id;
    std::shared_ptr<NodeDefinition> m_definition;
    std::array<float, 2> m_pos;
    std::array<float, 2> m_size;

    Node(const std::shared_ptr<NodeDefinition> &definition, const std::array<float, 2> &pos);

    ImColor GetBGColor(const Context &context, int node_selected) const;

    void DrawLeftPanel(int *node_selected, Context *context) const;

    ImVec2 GetInputSlotPos(int slot_no, float scaling) const;
    ImVec2 GetOutputSlotPos(int slot_no, float scaling) const;

    void Process(ImDrawList *draw_list, const ImVec2 &offset, Context *context, int *node_selected, float scaling);
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

class NodeDefinition;
class NodeScene
{
public:
    std::vector<std::shared_ptr<Node>> m_nodes;
    std::vector<std::shared_ptr<NodeLink>> m_links;

public:
    NodeScene();
    ~NodeScene();
    int GetIndex(const std::shared_ptr<Node> &node) const;
    std::shared_ptr<Node> CreateNode(const std::shared_ptr<NodeDefinition> &definition, float x, float y);
    std::shared_ptr<NodeLink> Link(const std::shared_ptr<Node> &src_node, int src_slot,
                                   const std::shared_ptr<Node> &dst_node, int dst_slot);
};

} // namespace plugnode