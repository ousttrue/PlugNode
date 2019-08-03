#pragma once
#include <memory>
#include <string>

namespace plugnode
{

class NodeDefinition
{
    // avoid creation
    NodeDefinition()
    {
    }

    friend class NodeGraphEditor;
};

class NodeGraphEditor
{
public:
    std::shared_ptr<NodeDefinition> CreateDefinition(const std::string &uniqueName) const;
};

} // namespace plugnode
