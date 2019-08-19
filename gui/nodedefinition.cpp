#include "nodedefinition.h"

namespace plugnode
{

std::shared_ptr<NodeDefinition> NodeDefinitionManager::Create(const std::string &name, const std::string &defaultName)
{
    auto node = std::make_shared<NodeDefinition>(name, defaultName);
    m_definitions.push_back(node);
    return node;
}

} // namespace plugnode