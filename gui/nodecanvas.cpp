#include "nodecanvas.h"
#include "context.h"
#include "node.h"
#include "nodescene.h"
#include "nodeslot/nodeslot.h"

#include <math.h>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>


namespace plugnode
{

class NodeCanvasImpl
{

public:
    void Show(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        context->ShowHeader();

        // Create our child canvas
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
        {
            ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
            // ImGui::PushItemWidth(120.0f * m_scaling);

            context->BeginScaling();
            ShowCanvas(context, definitions, scene);
            context->EndScaling();

            // ImGui::PopItemWidth();
            ImGui::EndChild();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

private:


    ///
    /// Canvas
    ///
    void ShowCanvas(Context *context, const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        context->BeginCanvas(draw_list);

        {
            draw_list->ChannelsSplit(2);
            // 0: Node contents
            // 1: Others(node rect, link curve...)

            // Display links
            draw_list->ChannelsSetCurrent(0);
            for (auto &node : scene->m_nodes)
            {
                for (auto &inSlot : node->m_inslots)
                {
                    inSlot->DrawLink(draw_list, context);
                }
            }

            context->DrawLink(draw_list);

            // Display nodes
            for (auto &node : scene->m_nodes)
            {
                // move, draw
                node->Process(draw_list, context);
            }
            draw_list->ChannelsMerge();
        }

        // {
        //     std::stringstream ss;
        //     ss << (old_any_active ? "active" : "false");
        //     ss << ", " << (node_widgets_active ? "active" : "false");
        //     ImGui::Begin("debug");
        //     ImGui::Text(ss.str().c_str());
        //     ImGui::End();
        // }

        // Open context menu
        context->ProcessClick(definitions, scene);

        context->EndCanvas();

    }
};

NodeCanvas::NodeCanvas()
    : m_impl(new NodeCanvasImpl)
{
}

NodeCanvas::~NodeCanvas()
{
    delete m_impl;
}

void NodeCanvas::Show(Context *context,
                      const NodeDefinitionManager *definitions,
                      NodeScene *scene)
{
    m_impl->Show(context, definitions, scene);
}

} // namespace plugnode