#include "nodedefinition.h"

namespace plugnode
{

NodeDefinition *NodeDefinitionManager::Create(const std::string &name)
{
    auto node = new NodeDefinition(name);
    m_definitions.push_back(node);
    return node;
}

} // namespace plugnode