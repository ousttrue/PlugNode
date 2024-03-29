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

class OutType : public OutSlot<TypeSlotType>
{
protected:
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        ImGui::LabelText(Name.c_str(), "" /*"##value"*/
                                          //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }

public:
    template <typename T>
    static std::shared_ptr<OutSlotBase> Create(const NodeSlotDefinition &definition)
    {
        auto p = new OutType;
        // std::stringstream ss;
        // ss << "##type:" << definition.name;
        p->Name = definition.name;
        p->TargetType = T();
        // p->Format = ss.str();
        return std::shared_ptr<OutSlotBase>(p);
    }
};

class OutFloatValue : public OutSlot<float>
{
protected:
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        *is_updated = ImGui::InputFloat(Name.c_str() /*"##value"*/,
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
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        *is_updated = ImGui::SliderFloat(Name.c_str() /*"##value"*/,
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
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        *is_updated = ImGui::ColorPicker4(Name.c_str() /*"##value"*/,
                            GetPinValue<std::array<float, 4>>()->data()
                            //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
        // ImGui::ColorEdit3("##color", &Color.x);
    }
};

void OutSlotBase::_UpdatePinPosition(float padding)
{
    GetPin()->Position = std::array<float, 2>{
        Rect[0] + Rect[2] + padding,
        Rect[1] + Rect[3] / 2};
}

} // namespace plugnode
