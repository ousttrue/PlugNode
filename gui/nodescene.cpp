#include "nodescene.h"
#include "nodedefinition.h"
#include "node.h"
#include "nodeslot/nodeslot.h"

namespace plugnode
{

NodeScene::NodeScene()
{
}

NodeScene::~NodeScene()
{
}

std::shared_ptr<Node> NodeScene::CreateNode(const std::shared_ptr<NodeDefinition> &definition, float x, float y)
{
    auto node = std::make_shared<Node>(definition, std::array<float, 2>{x, y});
    for (auto &slot : node->m_inslots)
    {
        slot->Owner = node;
        slot->GetPin()->Slot = slot;
    }
    for (auto &slot : node->m_outslots)
    {
        slot->Owner = node;
        slot->GetPin()->Slot = slot;
    }
    m_nodes.push_back(node);
    return node;
}

std::shared_ptr<Node> NodeScene::GetFromId(int id) const
{
    for (auto &node : m_nodes)
    {
        if (node->GetId() == id)
        {
            return node;
        }
    }
    return nullptr;
}

int NodeScene::GetIndex(const std::shared_ptr<Node> &node) const
{
    for (int i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i] == node)
        {
            return i;
        }
    }
    return -1;
}

void NodeScene::Link(const std::shared_ptr<Node> &src, int src_slot,
                     const std::shared_ptr<Node> &dst, int dst_slot)
{
    dst->m_inslots[dst_slot]->Link(src->m_outslots[src_slot]);
}

std::shared_ptr<OutSlotBase> NodeScene::GetHoverOutSlot() const
{
    for (auto &node : m_nodes)
    {
        for (auto &slot : node->m_outslots)
        {
            if (slot->IsHover)
            {
                return slot;
            }
        }
    }

    return nullptr;
}

std::shared_ptr<InSlotBase> NodeScene::GetHoverInSlot() const
{
    for (auto &node : m_nodes)
    {
        for (auto &slot : node->m_inslots)
        {
            if (slot->IsHover)
            {
                return slot;
            }
        }
    }

    return nullptr;
}
} // namespace plugnode