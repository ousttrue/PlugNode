#include "plugnode.h"
#include "nodegraph.h"
#include "nodedefinition.h"
#include "node.h"
#include "nodescene.h"
#include "nodeslot/nodeslot.h"
#include <perilune/perilune.h>

namespace perilune
{

template <>
struct LuaTable<plugnode::NodeSlotDefinition>
{
    static plugnode::NodeSlotDefinition Get(lua_State *L, int index)
    {
        auto table = LuaTableToTuple<std::string, std::string>(L, index);
        return plugnode::NodeSlotDefinition{
            std::get<0>(table),
            std::get<1>(table),
        };
    }
};

} // namespace perilune

void lua_require_plugnode(lua_State *L)
{
    auto top = lua_gettop(L);
    lua_newtable(L);

#pragma region definitions
    static perilune::UserType<plugnode::NodeSlotDefinition> nodeSocket;
    nodeSocket
        .StaticMethod("new", [](std::string name, std::string type) {
            return plugnode::NodeSlotDefinition{name, type};
        })
        .MetaMethod(perilune::MetaKey::__tostring, [](plugnode::NodeSlotDefinition *socket) {
            std::stringstream ss;
            ss << "[" << socket->type << "]" << socket->name;
            return ss.str();
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "node_socket");

    static perilune::UserType<std::vector<plugnode::NodeSlotDefinition> *>
        stringList;
    perilune::AddDefaultMethods(stringList);
    stringList
        .LuaNewType(L);
    lua_setfield(L, -2, "string_list");

    static perilune::UserType<std::shared_ptr<plugnode::NodeDefinition>> nodeDefinition;
    nodeDefinition
        .MetaIndexDispatcher([](auto d) {
            d->Getter("name", &plugnode::NodeDefinition::Name);
            d->Getter("inputs", [](plugnode::NodeDefinition *p) { return &p->Inputs; });
            d->Getter("outputs", [](plugnode::NodeDefinition *p) { return &p->Outputs; });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "definition");

    static perilune::UserType<plugnode::NodeDefinitionManager *> definitions;
    definitions
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", [](plugnode::NodeDefinitionManager *p, std::string name) {
                return p->Create(name);
            });
            d->Method("get_count", &plugnode::NodeDefinitionManager::GetCount);
            d->IndexGetter([](plugnode::NodeDefinitionManager *l, int i) {
                return l->Get(i - 1);
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "definition_manager");
#pragma endregion

#pragma region scene
    static perilune::UserType<std::shared_ptr<plugnode::InSlotBase>> inSlot;
    inSlot
        .MetaIndexDispatcher([](auto d) {
            d->Method("get_src_node", [](plugnode::InSlotBase *p) {
                return p->GetSrcNode();
            });
            d->LuaMethod("set_value", [](lua_State *L) {
                auto self = perilune::Traits<std::shared_ptr<plugnode::InSlotBase>>::GetSelf(L, lua_upvalueindex(2));
                auto &value = self->GetPin()->Value;
                auto x = (float)luaL_checknumber(L, 1);
                value = x;
                return 0;
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "inslot");

    static perilune::UserType<std::vector<std::shared_ptr<plugnode::InSlotBase>> *> inSlotList;
    inSlotList
        .LuaNewType(L);
    lua_setfield(L, -2, "inslot_list");

    static perilune::UserType<std::shared_ptr<plugnode::OutSlotBase>> outSlot;
    outSlot
        .MetaIndexDispatcher([](auto d) {
            d->LuaGetter("value", [](lua_State *L) {
                auto self = perilune::Traits<std::shared_ptr<plugnode::OutSlotBase>>::GetSelf(L, 1);
                auto &value = self->GetPin()->Value;
                if (value.type() == typeid(float))
                {
                    auto x = std::any_cast<float>(value);
                    return perilune::LuaPush<float>::Push(L, x);
                }
                else
                {
                    return 0;
                }
            });
            d->LuaMethod("set_value", [](lua_State *L) {
                auto self = perilune::Traits<std::shared_ptr<plugnode::OutSlotBase>>::GetSelf(L, lua_upvalueindex(2));
                auto &value = self->GetPin()->Value;
                auto x = (float)luaL_checknumber(L, 1);
                value = x;
                return 0;
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "outslot");

    static perilune::UserType<std::vector<std::shared_ptr<plugnode::OutSlotBase>> *> outSlotList;
    outSlotList
        .LuaNewType(L);
    lua_setfield(L, -2, "outslot_list");

    static perilune::UserType<std::shared_ptr<plugnode::Node>> node;
    node
        // .MetaMethod(perilune::MetaKey::__tostring, [](plugnode::Node *p) { return p->Name; })
        .MetaIndexDispatcher([](auto d) {
            d->Getter("is_dst", [](plugnode::Node *node) {
                return node->m_outslots.empty();
            });
            d->Getter("inslots", [](plugnode::Node *node) {
                return &node->m_inslots;
            });
            d->Getter("outslots", [](plugnode::Node *node) {
                return &node->m_outslots;
            });
            d->Getter("name", [](plugnode::Node *node) {
                return node->Name;
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "node");

    static perilune::UserType<plugnode::NodeScene *> nodescene;
    nodescene
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("create", [](plugnode::NodeScene *p, const std::shared_ptr<plugnode::NodeDefinition> &def, float x, float y) {
                return p->CreateNode(def, x, y);
            });
            d->Method("link", [](plugnode::NodeScene *p,
                                 const std::shared_ptr<plugnode::Node> &src, int src_slot,
                                 const std::shared_ptr<plugnode::Node> &dst, int dst_slot) {
                return p->Link(src, src_slot - 1, dst, dst_slot - 1);
            });
            d->IndexGetter([](plugnode::NodeScene *p, int i) {
                auto index = i - 1;
                if (index < 0 || index >= p->m_nodes.size())
                {
                    return std::shared_ptr<plugnode::Node>();
                }
                else
                {
                    return p->m_nodes[index];
                }
            });
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "scene");
#pragma endregion

    static perilune::UserType<plugnode::NodeGraph *> nodegraph;
    nodegraph
        .DefaultConstructorAndDestructor()
        .MetaIndexDispatcher([](auto d) {
            d->Method("imgui", &plugnode::NodeGraph::ImGui);
        })
        .LuaNewType(L);
    lua_setfield(L, -2, "graph");

    lua_setglobal(L, "plugnode");
    assert(top == lua_gettop(L));
}
