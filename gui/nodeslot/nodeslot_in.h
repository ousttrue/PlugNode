#pragma once
#include "nodeslot.h"
#include "context.h"
#include <memory>

namespace plugnode
{

std::shared_ptr<Node> InSlotBase::GetSrcNode()
{
    auto src = Src.lock();
    if (!src)
    {
        return nullptr;
    }

    auto srcSlot = src->Slot.lock();
    if (!srcSlot)
    {
        return nullptr;
    }

    auto node = srcSlot->Owner.lock();
    if (!node)
    {
        return nullptr;
    }

    return node;
}

void InSlotBase::DrawLink(ImDrawList *draw_list, Context *context)
{
    auto src = Src.lock();
    if (!src)
    {
        return;
    }

    auto p1 = *(ImVec2 *)&src->Position;
    auto p2 = *(ImVec2 *)&GetPin()->Position;

    draw_list->AddBezierCurve(
        p1,
        p1 + ImVec2(+50, 0),
        p2 + ImVec2(-50, 0),
        p2,
        IM_COL32(200, 200, 100, 255), context->GetLinkWidth());
}

template <typename T>
class InSlot : public InSlotBase
{
public:
    InSlot()
    {
        GetPin()->Value = T();
    }

    bool Acceptable(const std::shared_ptr<OutSlotBase> &src) override
    {
        auto srcPin = src->GetPin();
        if (typeid(T) == typeid(std::string))
        {
            return false;
        }
        return typeid(T) == srcPin->Value.type();
    }

    bool Link(const std::shared_ptr<OutSlotBase> &src) override
    {
        if (!Acceptable(src))
        {
            return false;
        }
        auto srcPin = src->GetPin();
        Src = srcPin;
        return true;
    }
};

class InType : public InSlot<TypeSlotType>
{
public:
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        ImGui::Text(Name.c_str());
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }

    bool Acceptable(const std::shared_ptr<OutSlotBase> &src) override
    {
        if (InSlot<TypeSlotType>::Acceptable(src))
        {
            // out: type => in: type
            return Name == src->Name;
        }
        else
        {
            // out: value => in: type
            return src->GetPin()->Value.type() == TargetType.type();
        }
    }

    template <typename T>
    static std::shared_ptr<InSlotBase> Create(const NodeSlotDefinition &definition)
    {
        auto p = new InType;
        // std::stringstream ss;
        // ss << "##type:" << definition.name;
        p->Name = definition.name;
        p->TargetType = T();
        // p->Format = ss.str();
        return std::shared_ptr<InSlotBase>(p);
    }
};

template <typename T>
class InLabelSlot : public InSlot<T>
{
public:
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        ImGui::Text(Name.c_str());
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

class InFloatValue : public InSlot<float>
{
public:
    std::array<float, 2> _OnImGui(Context *context, bool *is_updated) override
    {
        *is_updated = ImGui::InputFloat(Name.c_str() /*"##value"*/,
                                        GetPinValue<float>()
                                        //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

void InSlotBase::_UpdatePinPosition(float padding)
{
    GetPin()->Position = std::array<float, 2>{
        Rect[0] - padding,
        Rect[1] + Rect[3] / 2};
}

} // namespace plugnode