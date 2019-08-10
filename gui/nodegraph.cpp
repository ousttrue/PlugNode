#include "nodegraph.h"
#include "context.h"
#include "node.h"
#include "nodescene.h"
#include "nodecanvas.h"
#include <imgui.h>
#include <plog/Log.h>
#include <vector>

// Creating a node graph editor for ImGui
// Quick demo, not production code! This is more of a demo of how to use ImGui to create custom stuff.
// Better version by @daniel_collin here https://gist.github.com/emoon/b8ff4b4ce4f1b43e79f2
// See https://github.com/ocornut/imgui/issues/306
// v0.03: fixed grid offset issue, inverted sign of 'scrolling'
// Animated gif: https://cloud.githubusercontent.com/assets/8225057/9472357/c0263c04-4b4c-11e5-9fdf-2cd4f33f6582.gif

#include <math.h> // fmodf

namespace plugnode
{

class NodeGraphImpl
{
    Context m_context;
    NodeCanvas m_canvas;

public:
    NodeGraphImpl()
    {
    }

    void Show(const NodeDefinitionManager *definitions, NodeScene *scene)
    {
        m_context.NewFrame();

        {
            // 横幅を制限した描画
            ImGui::BeginChild("node_list", ImVec2(100, 0));
            ImGui::Text("Nodes");
            ImGui::Separator();

            for (auto &node : scene->m_nodes)
            {
                node->DrawLeftPanel(&m_context);
            }
            ImGui::EndChild();
        }

        {
            // 右隣に描画
            ImGui::SameLine();
            ImGui::BeginGroup();
            m_canvas.Show(&m_context, definitions, scene);
            ImGui::EndGroup();
        }
    }
};

NodeGraph::NodeGraph()
    : m_impl(new NodeGraphImpl)
{
}

NodeGraph::~NodeGraph()
{
    delete m_impl;
}

void NodeGraph::ImGui(NodeDefinitionManager *definitions,
                      NodeScene *scene)
{
    static bool s_show = true;
    if (ImGui::Begin("Example: Custom Node Graph", &s_show))
    {
        m_impl->Show(definitions, scene);
    }
    ImGui::End();
}

} // namespace plugnode
