#pragma once
#include "nodedefinition.h"
#include "node.h"

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

    void ImGui(NodeDefinitionManager *definitions,
               NodeScene *scene);
};

} // namespace plugnode