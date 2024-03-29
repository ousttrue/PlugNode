#pragma once
#include <vector>
#include <memory>

namespace plugnode
{

class Node;
class NodeDefinition;
class OutSlotBase;
class InSlotBase;
class NodeScene
{
public:
    std::vector<std::shared_ptr<Node>> m_nodes;

public:
    NodeScene();
    ~NodeScene();
    int GetIndex(const std::shared_ptr<Node> &node) const;
    std::shared_ptr<Node> GetFromId(int id) const;
    std::shared_ptr<Node> CreateNode(const std::shared_ptr<NodeDefinition> &definition, float x, float y);
    void Remove(const std::shared_ptr<Node> &node);
    void Link(const std::shared_ptr<Node> &src_node, int src_slot,
              const std::shared_ptr<Node> &dst_node, int dst_slot);
    std::shared_ptr<OutSlotBase> GetHoverOutSlot() const;
    std::shared_ptr<InSlotBase> GetHoverInSlot() const;
};

} // namespace plugnode