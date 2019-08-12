#pragma once

namespace plugnode
{
template <typename T>
class OutSlot : public OutSlotBase
{
public:
    OutSlot()
    {
        GetPin()->Value = T();
    }
};

class OutType : public OutSlot<std::string>
{
protected:
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::LabelText(Name.c_str(), "" /*"##value"*/
                                          //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }

public:
    static std::shared_ptr<OutSlotBase> Create(const NodeSlotDefinition &definition)
    {
        auto p = new OutType;
        // std::stringstream ss;
        // ss << "##type:" << definition.name;
        p->Name = definition.name;
        // p->Format = ss.str();
        return std::shared_ptr<OutSlotBase>(p);
    }
};

class OutFloatValue : public OutSlot<float>
{
protected:
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::InputFloat(Name.c_str() /*"##value"*/,
                          GetPinValue<float>()
                          //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

class OutFloatSlider : public OutSlot<float>
{
public:
    std::string Format;
    float Min = 0;
    float Max = 1.0f;
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::SliderFloat(Name.c_str() /*"##value"*/,
                           GetPinValue<float>(), Min, Max
                           //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
        // ImGui::ColorEdit3("##color", &Color.x);
    }
};

class OutFloat4Color : public OutSlot<std::array<float, 4>>
{
public:
    // std::string Format;
    // float Min = 0;
    // float Max = 1.0f;
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::ColorPicker4(Name.c_str() /*"##value"*/,
                            GetPinValue<std::array<float, 4>>()->data()
                            //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
        // ImGui::ColorEdit3("##color", &Color.x);
    }
};
void OutSlotBase::_UpdatePinPosition()
{
    GetPin()->Position = std::array<float, 2>{
        Rect[0] + Rect[2] + NODE_WINDOW_PADDING.x,
        Rect[1] + Rect[3] / 2};
}


}
