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
    void LoadDefinitions(const class NodeDefinition *p, int len);
    void ShowGui();
};

} // namespace plugnode