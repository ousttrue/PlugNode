#include "nodeslot.h"
#include "nodedefinition.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <sstream>

const float NODE_SLOT_RADIUS = 6.0f;
const float SQ_NODE_SLOT_RADIUS = NODE_SLOT_RADIUS * NODE_SLOT_RADIUS;
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
const auto PIN_COLOR = IM_COL32(150, 150, 150, 150);
const auto PIN_HOVER_COLOR = IM_COL32(150, 250, 150, 150);

#include "nodeslot_out.h"
#include "nodeslot_in.h"

namespace plugnode
{

NodeSlotBase::NodeSlotBase()
    : Pin(new NodePin)
{
}

void NodeSlotBase::ImGui(ImDrawList *draw_list, float scale)
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    auto size = _OnImGui(scale);
    Rect[2] = size[0];
    Rect[3] = size[1];
    _UpdatePinPosition();
    _DrawPin(draw_list, scale);
}

static float Dot(const ImVec2 &v)
{
    return v.x * v.x + v.y * v.y;
}

void NodeSlotBase::_DrawPin(ImDrawList *draw_list, float scaling)
{
    auto pos = *(ImVec2 *)&GetPin()->Position;
    auto mouse = ImGui::GetMousePos();
    auto dot = Dot(pos - mouse);
    auto r = NODE_SLOT_RADIUS * scaling;
    IsHover = dot <= r * r;
    if (IsHover)
    {
        // on mouse
        draw_list->AddCircleFilled(pos, r, PIN_HOVER_COLOR);
    }
    else
    {
        draw_list->AddCircleFilled(pos, r, PIN_COLOR);
    }
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
        return OutType::Create(definition);
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
        return InType::Create(definition);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

} // namespace plugnode
