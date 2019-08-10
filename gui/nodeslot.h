#pragma once
#include <string>
#include <memory>
#include <array>

namespace plugnode
{

struct NodeSocket;
class NodeSlot
{
public:
    std::array<float, 4> Rect;
    std::array<float, 2> GetLinkPosition() const
    {
        return std::array<float, 2>{
            Rect[0] + Rect[2],
            Rect[1] + Rect[3] / 2};
    }
    virtual void ImGui(){};
    static std::shared_ptr<NodeSlot> CreateOut(const NodeSocket &out);
    static std::shared_ptr<NodeSlot> CreateIn(const NodeSocket &in);
};

template <typename T>
class ValueSlot : public NodeSlot
{
public:
    std::string Name;
    T Value = 0;
};

class FloatValue : public ValueSlot<float>
{
    void ImGui() override;
};

class FloatSlider : public FloatValue
{
public:
    std::string Format;
    float Min = 0;
    float Max = 1.0f;
    void ImGui() override;
};

} // namespace plugnode