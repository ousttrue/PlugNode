#include "nodeslot.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <sstream>

namespace plugnode
{

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

std::shared_ptr<NodeSlot> NodeSlot::Create(const NodeSocket &out)
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