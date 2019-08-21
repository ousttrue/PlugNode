#pragma once
#include <array>
#include <memory>
#include <any>

struct ImDrawList;
namespace plugnode
{

struct TypeSlotType
{
};

class NodeSlotBase;
class Node;
class Context;
struct NodeSlotDefinition;

struct NodePin
{
    std::array<float, 2> Position;
    std::any Value;
    std::weak_ptr<NodeSlotBase> Slot;
};

class NodeSlotBase
{
    std::shared_ptr<NodePin> Pin;

protected:
    virtual std::array<float, 2> _OnImGui(Context *context, bool *is_updated) = 0;
    virtual void _UpdatePinPosition(float padding) = 0;
    NodeSlotBase();
    template <typename T>
    T *GetPinValue()
    {
        return std::any_cast<T>(&Pin->Value);
    }

public:
    std::weak_ptr<Node> Owner;
    bool IsHover = false;
    const std::shared_ptr<NodePin> &GetPin() const { return Pin; }
    std::string Name;
    std::any TargetType;
    std::array<float, 4> Rect;
    bool ImGui(ImDrawList *draw_list, Context *context);
};

enum class SlotType
{
    Value,
    LabelOnly,
    UserInterface,
};

class OutSlotBase : public NodeSlotBase
{
protected:
    void _UpdatePinPosition(float padding) override;

public:
    static std::shared_ptr<OutSlotBase> Create(const NodeSlotDefinition &socket, SlotType slotType);
};

class InSlotBase : public NodeSlotBase
{
protected:
    std::weak_ptr<NodePin> Src;
    void _UpdatePinPosition(float padding) override;

public:
    void DrawLink(ImDrawList *draw_list, Context *context);
    virtual bool Acceptable(const std::shared_ptr<OutSlotBase> &src) = 0;
    virtual bool Link(const std::shared_ptr<OutSlotBase> &src) = 0;

    bool Disconnect()
    {
        auto src = Src.lock();
        if (!src)
        {
            return false;
        }
        Src.reset();
        return true;
    }
    std::shared_ptr<Node> GetSrcNode();

    static std::shared_ptr<InSlotBase> Create(const NodeSlotDefinition &socket, SlotType slotType);
};

} // namespace plugnode