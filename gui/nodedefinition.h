#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace plugnode
{

struct NodeSlotDefinition
{
    std::string name;
    std::string type;
};

class NodeDefinition
{
    NodeDefinition(const NodeDefinition &) = delete;
    NodeDefinition &operator=(const NodeDefinition &) = delete;

public:
    NodeDefinition(const std::string &name)
        : Name(name)
    {
    }

    ~NodeDefinition()
    {
    }

    std::string Name;
    std::vector<NodeSlotDefinition> Inputs;
    std::vector<NodeSlotDefinition> Outputs;

    friend class NodeDefinitionManager;
};

class NodeDefinitionManager
{
public:
    std::vector<std::shared_ptr<NodeDefinition>> m_definitions;

public:
    std::shared_ptr<NodeDefinition> Create(const std::string &name);
    int GetCount() const { return (int)m_definitions.size(); }
    std::shared_ptr<NodeDefinition> Get(int index)
    {
        if (index < 0 || index >= m_definitions.size())
            return nullptr;
        return m_definitions[index];
    }
};

} // namespace plugnode