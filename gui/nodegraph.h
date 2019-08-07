#pragma once

namespace plugnode
{

class NodeGraphImpl;
class NodeGraph
{
    NodeGraphImpl *m_impl = nullptr;

public:
    NodeGraph();
    ~NodeGraph();
    void ClearDefinitions();
    void AddDefinition(const class NodeDefinition *p);
    void ShowGui();
};

} // namespace plugnode