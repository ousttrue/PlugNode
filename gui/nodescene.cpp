#include "nodescene.h"
#include "nodedefinition.h"
#include "node.h"

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

std::shared_ptr<NodeLink> NodeScene::Link(const std::shared_ptr<Node> &src, int src_slot,
                                          const std::shared_ptr<Node> &dst, int dst_slot)
{
    // auto src_index = GetIndex(src_node);
    // if (src_index == -1)
    // {
    //     return nullptr;
    // }
    // auto dst_index = GetIndex(dst_node);
    // if (dst_index == -1)
    // {
    //     return nullptr;
    // }
    auto link = std::make_shared<NodeLink>(src->GetId(), src_slot, dst->GetId(), dst_slot);
    m_links.push_back(link);
    return link;
}

std::array<float, 2> NodeScene::GetLinkSrc(const std::shared_ptr<NodeLink> &link, float scaling)
{
    auto &src = GetFromId(link->SrcNode);
    // return src->GetOutLinkPosition(link->SrcSlot);

    //         auto pos = slot->GetLinkPosition();
    return src->GetOutputSlotPos(link->SrcSlot, scaling);
}

} // namespace plugnode