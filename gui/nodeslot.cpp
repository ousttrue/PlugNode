#include "nodeslot.h"
// #include "node.h"
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

std::shared_ptr<OutSlotBase> OutSlotBase::CreateValue(const NodeSlotDefinition &definition)
{
    if (definition.type == "float")
    {
        auto p = new OutFloatValue;
        // std::stringstream ss;
        p->Name = definition.name;
        // p->Format = ss.str();
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

std::shared_ptr<OutSlotBase> OutSlotBase::CreateGui(const NodeSlotDefinition &definition)
{
    if (definition.type == "float")
    {
        auto p = new OutFloatSlider;
        std::stringstream ss;
        ss << definition.name << " %.2f";
        p->Name = definition.name;
        p->Format = ss.str();
        return std::shared_ptr<OutSlotBase>(p);
    }
    if (definition.type == "float4")
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
#pragma endregion

#pragma region InSlot
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

    bool Link(const std::shared_ptr<OutSlotBase> &src) override
    {
        auto srcPin = src->GetPin();
        if (typeid(T) != srcPin->Value.type())
        {
            return false;
        }
        Src = srcPin;
        return true;
    }
};

class InType : public InSlot<std::string>
{
public:
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::Text(Name.c_str());
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }

    static std::shared_ptr<InSlotBase> Create(const NodeSlotDefinition &definition)
    {
        auto p = new InType;
        // std::stringstream ss;
        // ss << "##type:" << definition.name;
        p->Name = definition.name;
        // p->Format = ss.str();
        return std::shared_ptr<InSlotBase>(p);
    }
};

template <typename T>
class InLabelSlot : public InSlot<T>
{
public:
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::Text(Name.c_str());
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

class InFloatValue : public InSlot<float>
{
public:
    std::array<float, 2> _OnImGui(float scale) override
    {
        ImGui::InputFloat(Name.c_str() /*"##value"*/,
                          GetPinValue<float>()
                          //, Format.c_str()
        );
        return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    }
};

void InSlotBase::_UpdatePinPosition()
{
    GetPin()->Position = std::array<float, 2>{
        Rect[0] - NODE_WINDOW_PADDING.x,
        Rect[1] + Rect[3] / 2};
}

std::shared_ptr<InSlotBase> InSlotBase::CreateValue(const NodeSlotDefinition &definition)
{
    if (definition.type == "float")
    {
        auto p = new InFloatValue;
        std::stringstream ss;
        ss << "##" << definition.name;
        p->Name = ss.str();
        // p->Format = ss.str();
        return std::shared_ptr<InSlotBase>(p);
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

std::shared_ptr<InSlotBase> InSlotBase::CreateLabel(const NodeSlotDefinition &socket)
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
