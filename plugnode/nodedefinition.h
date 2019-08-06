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

    friend class NodeManager;
};

class NodeManager
{
    std::vector<NodeDefinition *> m_nodes;

    NodeManager(const NodeManager &) = delete;
    NodeManager &operator=(const NodeManager &) = delete;

public:
    NodeManager()
    {
    }

    ~NodeManager()
    {
        for (auto node : m_nodes)
        {
            delete node;
        }
    }

    NodeDefinition *AddNode(const std::string &name)
    {
        auto node = new NodeDefinition(name);
        m_nodes.push_back(node);
        return node;
    }

    int GetCount() const { return static_cast<int>(m_nodes.size()); }

    NodeDefinition *GetNode(int index) const
    {
        if (index < 0)
            return nullptr;
        if (index >= GetCount())
            return nullptr;
        return m_nodes[index];
    }
};

} // namespace plugnode