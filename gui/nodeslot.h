#pragma once
#include <array>
#include <memory>
#include <any>

struct ImDrawList;
namespace plugnode
{

struct NodePin
{
    std::array<float, 2> Position;
    std::any Value;
};

struct NodeSocket;
class NodeSlotBase
{
    std::shared_ptr<NodePin> Pin;

protected:
    virtual std::array<float, 2> _OnImGui() = 0;
    void _DrawPin(ImDrawList *draw_list);
    virtual void _UpdatePinPosition() = 0;
    NodeSlotBase();
    template <typename T>
    T *GetPinValue()
    {
        return std::any_cast<T>(&Pin->Value);
    }

public:
    bool IsHover = false;
    const std::shared_ptr<NodePin> &GetPin() const { return Pin; }
    std::string Name;
    std::array<float, 4> Rect;
    void ImGui(ImDrawList *draw_list);
};

class OutSlotBase : public NodeSlotBase
{
protected:
    void _UpdatePinPosition() override;

public:
    static std::shared_ptr<OutSlotBase> CreateValue(const NodeSocket &socket);
    static std::shared_ptr<OutSlotBase> CreateGui(const NodeSocket &socket);
};

class InSlotBase : public NodeSlotBase
{
protected:
    std::weak_ptr<NodePin> Src;
    void _UpdatePinPosition() override;

public:
    void DrawLink(ImDrawList *draw_list, float width);
    virtual bool Link(const std::shared_ptr<OutSlotBase> &src) = 0;
    void Disconnect()
    {
        Src.reset();
    }

    static std::shared_ptr<InSlotBase> CreateValue(const NodeSocket &socket);
    static std::shared_ptr<InSlotBase> CreateLabel(const NodeSocket &socket);
};

} // namespace plugnode