#include "nodeslot.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <sstream>

namespace plugnode
{

void FloatValue::ImGui()
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    ImGui::InputFloat(Name.c_str() /*"##value"*/,
                       &Value
                       //, Format.c_str()
    );
    auto size = ImGui::GetItemRectSize();
    Rect[2] = size.x;
    Rect[3] = size.y;
    // ImGui::ColorEdit3("##color", &Color.x);
}

void FloatSlider::ImGui()
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    ImGui::SliderFloat(Name.c_str() /*"##value"*/,
                       &Value, Min, Max
                       //, Format.c_str()
    );
    auto size = ImGui::GetItemRectSize();
    Rect[2] = size.x;
    Rect[3] = size.y;
    // ImGui::ColorEdit3("##color", &Color.x);
}

//////////////////////////////////////////////////////////////////////////////
// factory
//////////////////////////////////////////////////////////////////////////////
std::shared_ptr<NodeSlot> NodeSlot::CreateIn(const NodeSocket &in)
{
    if (in.type == "float")
    {
        auto p = new FloatValue;
        std::stringstream ss;
        ss << in.name << " %.2f";
        p->Name = in.name;
        // p->Format = ss.str();
        return std::shared_ptr<NodeSlot>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<NodeSlot> NodeSlot::CreateOut(const NodeSocket &out)
{
    if (out.type == "float")
    {
        auto p = new FloatSlider;
        std::stringstream ss;
        ss << out.name << " %.2f";
        p->Name = out.name;
        p->Format = ss.str();
        return std::shared_ptr<NodeSlot>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

} // namespace plugnode