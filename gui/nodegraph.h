#pragma once

struct lua_State;

namespace plugnode
{

class NodeGraphImpl;
class NodeGraph
{
    NodeGraphImpl *m_impl = nullptr;

public:
    NodeGraph();
    ~NodeGraph();

    bool ImGui(class NodeDefinitionManager *definitions,
               class NodeScene *scene);
};

} // namespace plugnode