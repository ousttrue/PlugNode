#pragma once
#include <string>
#include <memory>

namespace plugnode
{

struct NodeSocket;
class IOutSlot
{
public:
    virtual void ImGui(){};
    static std::shared_ptr<IOutSlot> IOutSlot::Create(const NodeSocket &out);
};

template <typename T>
class ValueSlot : public IOutSlot
{
public:
    std::string Name;
    T Value = 0;
};

class FloatSlider : public ValueSlot<float>
{
public:
    std::string Format;
    float Min = 0;
    float Max = 1.0f;
    void ImGui() override;
};

}