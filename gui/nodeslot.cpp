#include "nodeslot.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <sstream>

namespace plugnode
{

void FloatSlider::ImGui()
{
    ImGui::SliderFloat(Name.c_str() /*"##value"*/,
                       &Value, Min, Max
                       //, Format.c_str()
    );
    // ImGui::ColorEdit3("##color", &Color.x);
}

std::shared_ptr<IOutSlot> IOutSlot::Create(const NodeSocket &out)
{
    if (out.type == "float")
    {
        auto p = new FloatSlider;
        std::stringstream ss;
        ss << out.name << " %.2f";
        p->Name = out.name;
        p->Format = ss.str();
        return std::shared_ptr<IOutSlot>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

} // namespace plugnode