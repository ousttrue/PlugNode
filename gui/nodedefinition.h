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

    friend class NodeGraph;
};

} // namespace plugnode