#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace plugnode
{

struct NodeSocket
{
    std::string name;
    std::string type;
};

class NodeDefinition
{
    NodeDefinition(const NodeDefinition &) = delete;
    NodeDefinition &operator=(const NodeDefinition &) = delete;

    NodeDefinition(const std::string &name)
        : Name(name)
    {
    }

    ~NodeDefinition()
    {
    }

public:
    std::string Name;
    std::vector<NodeSocket> Inputs;
    std::vector<NodeSocket> Outputs;

    friend class NodeDefinitionManager;
};

class NodeDefinitionManager
{
public:
    std::vector<NodeDefinition *> m_definitions;

public:
    NodeDefinition *Create(const std::string &name);
    int GetCount() const { return (int)m_definitions.size(); }
    NodeDefinition *Get(int index)
    {
        if (index < 0 || index >= m_definitions.size())
            return nullptr;
        return m_definitions[index];
    }
};

} // namespace plugnode