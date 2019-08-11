#include "nodeslot.h"
#include "nodedefinition.h"
#include <imgui.h>
#include <sstream>

const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
const float NODE_SLOT_RADIUS = 6.0f;

namespace plugnode
{

void NodeSlot::ImGui(ImDrawList *draw_list)
{
    auto pos = ImGui::GetCursorScreenPos();
    Rect[0] = pos.x;
    Rect[1] = pos.y;
    auto size = OnImGui();
    Rect[2] = size[0];
    Rect[3] = size[1];
    DrawSocket(draw_list);
}
void NodeSlot::DrawSocket(ImDrawList *draw_list)
{
    // auto draw_list = ImGui::GetWindowDrawList();
    auto pos = *(ImVec2 *)&GetLinkPosition();
    draw_list->AddCircleFilled(pos, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
}

std::array<float, 2> LabelSlot::OnImGui()
{
    ImGui::Text(Name.c_str());
    return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
}

std::array<float, 2> FloatValue::OnImGui()
{
    ImGui::PushItemWidth(80);
    ImGui::InputFloat(Name.c_str() /*"##value"*/,
                      &Value
                      //, Format.c_str()
    );
    ImGui::PopItemWidth();
    return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
}

std::array<float, 2> FloatSlider::OnImGui()
{
    ImGui::SliderFloat(Name.c_str() /*"##value"*/,
                       &Value, Min, Max
                       //, Format.c_str()
    );
    return *(std::array<float, 2> *)&ImGui::GetItemRectSize();
    // ImGui::ColorEdit3("##color", &Color.x);
}

std::array<float, 2> NodeSlot::GetLinkPosition() const
{
    switch (InOut)
    {
    case NodeSlotInOut::In:
        return std::array<float, 2>{
            Rect[0] - NODE_WINDOW_PADDING.x,
            Rect[1] + Rect[3] / 2};
    case NodeSlotInOut::Out:
        return std::array<float, 2>{
            Rect[0] + Rect[2] + NODE_WINDOW_PADDING.x,
            Rect[1] + Rect[3] / 2};
    }

    throw std::exception("no direction");
}

//////////////////////////////////////////////////////////////////////////////
// factory
//////////////////////////////////////////////////////////////////////////////
std::shared_ptr<NodeSlot> NodeSlot::CreateGui(const NodeSocket &socket, NodeSlotInOut inout)
{
    if (socket.type == "float")
    {
        auto p = new FloatSlider(inout);
        std::stringstream ss;
        ss << socket.name << " %.2f";
        p->Name = socket.name;
        p->Format = ss.str();
        return std::shared_ptr<NodeSlot>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<NodeSlot> NodeSlot::CreateValue(const NodeSocket &socket, NodeSlotInOut inout)
{
    if (socket.type == "float")
    {
        auto p = new FloatValue(inout);
        std::stringstream ss;
        ss << "##" << socket.name;
        p->Name = ss.str();
        // p->Format = ss.str();
        return std::shared_ptr<NodeSlot>(p);
    }
    else
    {
        throw std::exception("not implemented");
    }
}

std::shared_ptr<NodeSlot> NodeSlot::CreateLabel(const NodeSocket &socket, NodeSlotInOut inout)
{
    auto p = new LabelSlot(inout);
    p->Name = socket.name;
    return std::shared_ptr<NodeSlot>(p);
}

} // namespace plugnode