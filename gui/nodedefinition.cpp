#include "nodedefinition.h"

namespace plugnode
{

std::shared_ptr<NodeDefinition> NodeDefinitionManager::Create(const std::string &name)
{
    auto node = std::make_shared<NodeDefinition>(name);
    m_definitions.push_back(node);
    return node;
}

} // namespace plugnode