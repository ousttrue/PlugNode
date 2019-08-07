#pragma once
#include <string>

namespace plugnode
{

class NodeDefinition;
class NodeGraphImpl;
class NodeGraph
{
    NodeGraphImpl *m_impl = nullptr;

public:
    NodeGraph();
    ~NodeGraph();

    void ClearDefinitions();
    NodeDefinition *CreateDefinition(const std::string &name);
    int GetDefinitionCount() const;
    NodeDefinition *GetDefinition(int index) const;

    void ShowGui();
};

} // namespace plugnode