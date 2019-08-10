#include "context.h"
#include "nodedefinition.h"
#include "node.h"

namespace plugnode
{

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
            ImGui::Text("Node '%s'", node->m_definition->Name.c_str());
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