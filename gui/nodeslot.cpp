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

namespace plugnode
{

NodeSlotBase::NodeSlotBase()
    : Pin(new NodePin)
{
}

void NodeSlotBase::ImGui(ImDrawList *draw_list)
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    auto size = _OnImGui();
    Rect[2] = size[0];
    Rect[3] = size[1];
    _UpdatePinPosition();
    _DrawPin(draw_list);
}

static float Dot(const ImVec2 &v)
{
    return v.x * v.x + v.y * v.y;
}
void NodeSlotBase::_DrawPin(ImDrawList *draw_list)
{
    auto pos = *(ImVec2 *)&GetPin()->Position;
    auto mouse = ImGui::GetMousePos();
    auto dot = Dot(pos - mouse);
    if (dot <= SQ_NODE_SLOT_RADIUS)
    {
        // on mouse
        draw_list->AddCircleFilled(pos, NODE_SLOT_RADIUS, PIN_HOVER_COLOR);
    }
    else
    {
        draw_list->AddCircleFilled(pos, NODE_SLOT_RADIUS, PIN_COLOR);
    }
}

#pragma region OutSlot
template <typename T>
class OutSlot : public OutSlotBase
{
public:
    OutSlot()
    {
        GetPin()->Value = T();
    }
};

class OutFloatValue : public OutSlot<float>
{
protected:
    std::array<float, 2> _OnImGui() override
    {
        ImGui::PushItemWidth(80);
        ImGui::InputFloat(Name.c_str() /*"##value"*/,
                          GetPinValue<float>()
                          //, Format.c_str()
        );
        ImGui::PopItemWidth();
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

class OutFloatSlider : public OutSlot<float>
{
public:
    std::string Format;
    float Min = 0;
    float Max = 1.0f;
    std::array<float, 2> _OnImGui() override
    {
        ImGui::SliderFloat(Name.c_str() /*"##value"*/,
                           GetPinValue<float>(), Min, Max
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

std::shared_ptr<OutSlotBase> OutSlotBase::CreateValue(const NodeSocket &socket)
{
    if (socket.type == "float")
    {
        auto p = new OutFloatValue;
        std::stringstream ss;
        ss << "##" << socket.name;
        p->Name = ss.str();
        // p->Format = ss.str();
        return std::shared_ptr<OutSlotBase>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<OutSlotBase> OutSlotBase::CreateGui(const NodeSocket &socket)
{
    if (socket.type == "float")
    {
        auto p = new OutFloatSlider;
        std::stringstream ss;
        ss << socket.name << " %.2f";
        p->Name = socket.name;
        p->Format = ss.str();
        return std::shared_ptr<OutSlotBase>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}
#pragma endregion

#pragma region InSlot
void InSlotBase::DrawLink(ImDrawList *draw_list, float width)
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
        IM_COL32(200, 200, 100, 255), width);
}

template <typename T>
class InSlot : public InSlotBase
{
public:
    InSlot()
    {
        GetPin()->Value = T();
    }

    void Link(const std::shared_ptr<OutSlotBase> &src) override
    {
        auto srcPin = src->GetPin();
        if (typeid(T) == srcPin->Value.type())
        {
            Src = srcPin;
        }
    }
};

template <typename T>
class InLabelSlot : public InSlot<T>
{
public:
    std::array<float, 2> _OnImGui() override
    {
        ImGui::Text(Name.c_str());
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

class InFloatValue : public InSlot<float>
{
public:
    std::array<float, 2> _OnImGui() override
    {
        ImGui::PushItemWidth(80);
        ImGui::InputFloat(Name.c_str() /*"##value"*/,
                          GetPinValue<float>()
                          //, Format.c_str()
        );
        ImGui::PopItemWidth();
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

void InSlotBase::_UpdatePinPosition()
{
    GetPin()->Position = std::array<float, 2>{
        Rect[0] - NODE_WINDOW_PADDING.x,
        Rect[1] + Rect[3] / 2};
}

std::shared_ptr<InSlotBase> InSlotBase::CreateValue(const NodeSocket &socket)
{
    if (socket.type == "float")
    {
        auto p = new InFloatValue;
        std::stringstream ss;
        ss << "##" << socket.name;
        p->Name = ss.str();
        // p->Format = ss.str();
        return std::shared_ptr<InSlotBase>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<InSlotBase> InSlotBase::CreateLabel(const NodeSocket &socket)
{
    if (socket.type == "float")
    {
        auto p = new InLabelSlot<float>;
        p->Name = socket.name;
        return std::shared_ptr<InSlotBase>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}
#pragma endregion

} // namespace plugnode
