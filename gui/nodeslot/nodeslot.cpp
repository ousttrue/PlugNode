#include "nodeslot.h"
#include "nodedefinition.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "nodeslot_out.h"
#include "nodeslot_in.h"

#include <sstream>

namespace plugnode
{

NodeSlotBase::NodeSlotBase()
    : Pin(new NodePin)
{
}

bool NodeSlotBase::ImGui(ImDrawList *draw_list, Context *context)
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    auto is_updated = false;
    auto size = _OnImGui(context, &is_updated);
    Rect[2] = size[0];
    Rect[3] = size[1];
    _UpdatePinPosition(context->GetNodeHorizontalPadding());

    auto pin = GetPin()->Position;
    IsHover = context->DrawPin(draw_list, pin[0], pin[1]);
    return is_updated;
}

std::shared_ptr<OutSlotBase> OutSlotBase::Create(const NodeSlotDefinition &definition, SlotType slotType)
{
    if (definition.type == "float")
    {
        switch (slotType)
        {
        case SlotType::Value:
        {
            auto p = new OutFloatValue;
            // std::stringstream ss;
            p->Name = definition.name;
            // p->Format = ss.str();
            return std::shared_ptr<OutSlotBase>(p);
        }

        case SlotType::UserInterface:
        {
            auto p = new OutFloatSlider;
            std::stringstream ss;
            ss << definition.name << " %.2f";
            p->Name = definition.name;
            p->Format = ss.str();
            return std::shared_ptr<OutSlotBase>(p);
        }
        }
        throw std::exception("not implemented");
    }
    else if (definition.type == "float4")
    {
        auto p = new OutFloat4Color;
        p->Name = definition.name;
        return std::shared_ptr<OutSlotBase>(p);
    }
    else if (definition.type == "type")
    {
        if (definition.name == "float3_t")
        {
            return OutType::Create<std::array<float, 3>>(definition);
        }
        else if (definition.name == "float4_t")
        {
            return OutType::Create<std::array<float, 4>>(definition);
        }
        else
        {
            throw std::exception("not implemented");
        }
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<InSlotBase> InSlotBase::Create(const NodeSlotDefinition &definition, SlotType slotType)
{
    if (definition.type == "float")
    {
        switch (slotType)
        {
        case SlotType::Value:
        {
            auto p = new InFloatValue;
            std::stringstream ss;
            ss << "##" << definition.name;
            p->Name = ss.str();
            // p->Format = ss.str();
            return std::shared_ptr<InSlotBase>(p);
        }

        case SlotType::LabelOnly:
        {
            auto p = new InLabelSlot<float>;
            p->Name = definition.name;
            return std::shared_ptr<InSlotBase>(p);
        }
        }

        throw std::exception("not implemented");
    }
    else if (definition.type == "type")
    {
        if (definition.name == "float3_t")
        {
            return InType::Create<std::array<float, 3>>(definition);
        }
        else if (definition.name == "float4_t")
        {
            return InType::Create<std::array<float, 4>>(definition);
        }
        else
        {
            throw std::exception("not implemented");
        }
    }
    else
    {
        throw std::exception("not implemented");
    }
}

} // namespace plugnode
