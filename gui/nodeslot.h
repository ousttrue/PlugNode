#pragma once
#include <string>
#include <memory>
#include <array>

namespace plugnode
{

enum class NodeSlotInOut
{
    In,
    Out,
};

struct NodeSocket;
class NodeSlot
{
protected:
    NodeSlotInOut InOut;
    NodeSlot(NodeSlotInOut inout) : InOut(inout) {}
    std::string Name;

public:
    std::array<float, 4> Rect;
    std::array<float, 2> GetLinkPosition() const;
    void ImGui();
    virtual std::array<float, 2> OnImGui() = 0;

    static std::shared_ptr<NodeSlot> CreateGui(const NodeSocket &socket, NodeSlotInOut inout);
    static std::shared_ptr<NodeSlot> CreateValue(const NodeSocket &socket, NodeSlotInOut inout);
    static std::shared_ptr<NodeSlot> CreateLabel(const NodeSocket &socket, NodeSlotInOut inout);
};

class LabelSlot : public NodeSlot
{
public:
    LabelSlot(NodeSlotInOut inout) : NodeSlot(inout) {}
    std::array<float, 2> OnImGui() override;
};

template <typename T>
class ValueSlot : public NodeSlot
{
public:
    ValueSlot(NodeSlotInOut inout) : NodeSlot(inout) {}
    T Value = 0;
};

class FloatValue : public ValueSlot<float>
{
public:
    FloatValue(NodeSlotInOut inout) : ValueSlot(inout) {}
    std::array<float, 2> OnImGui() override;
};

class FloatSlider : public FloatValue
{
public:
    FloatSlider(NodeSlotInOut inout) : FloatValue(inout) {}
    std::string Format;
    float Min = 0;
    float Max = 1.0f;
    std::array<float, 2> OnImGui() override;
};

} // namespace plugnode