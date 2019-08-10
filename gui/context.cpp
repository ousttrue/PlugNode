#include "context.h"
#include "nodedefinition.h"
#include "node.h"
#include "nodescene.h"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

ImU32 NODE_COLOR = IM_COL32(60, 60, 60, 255);
ImU32 NODE_HOVER_COLOR = IM_COL32(75, 75, 90, 255);

namespace plugnode
{

void Context::HoverInList(int id)
{
    m_node_hovered_in_list = id;
    if (ImGui::IsMouseClicked(1))
    {
        // right click
        m_open_context_menu = true;
    }
}

void Context::HoverInScene(int id)
{
    m_node_hovered_in_scene = id;
    if (ImGui::IsMouseClicked(1))
    {
        // right click
        m_open_context_menu = true;
    }
}

bool Context::IsHovered(int ID) const
{
    return m_node_hovered_in_list == ID || m_node_hovered_in_scene == ID;
}

bool Context::IsSelected(int ID) const
{
    return m_node_hovered_in_list == -1 && m_node_selected == ID;
}

ImU32 Context::GetBGColor(int m_id) const
{
    if (IsHovered(m_id) || IsSelected(m_id))
    {
        return NODE_HOVER_COLOR;
    }
    else
    {
        return NODE_COLOR;
    }
}

void Context::ContextMenu(const ImVec2 &offset,
                          const NodeDefinitionManager *definitions,
                          NodeScene *scene)
{
    if (!ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
    {
        // right click on canvas
        m_node_selected = m_node_hovered_in_list = m_node_hovered_in_scene = -1;
        m_open_context_menu = true;
    }

    if (m_open_context_menu)
    {
        ImGui::OpenPopup("context_menu");
        if (m_node_hovered_in_list != -1)
            m_node_selected = m_node_hovered_in_list;
        if (m_node_hovered_in_scene != -1)
            m_node_selected = m_node_hovered_in_scene;
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("context_menu"))
    {
        ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (m_node_selected != -1)
        {
            auto node = scene->GetFromId(m_node_selected);
            ImGui::Text("Node '%s'", node->GetName().c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("Rename..", NULL, false, false))
            {
            }
            if (ImGui::MenuItem("Delete", NULL, false, false))
            {
            }
            if (ImGui::MenuItem("Copy", NULL, false, false))
            {
            }
        }
        else
        {
            for (auto p : definitions->m_definitions)
            {
                if (ImGui::MenuItem(p->Name.c_str()))
                {
                    scene->CreateNode(p, scene_pos[0], scene_pos[1]);
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

} // namespace plugnode