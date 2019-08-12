------------------------------------------------------------------------------
-- node definitions
------------------------------------------------------------------------------
function printNode(index, node)
    print(string.format('[%s: %s]', index, node.name))
    if #node.inputs > 0 then
        print('input{')
        for i, input in ipairs(node.inputs) do print(i, input) end
        print('}')
    end

    if #node.outputs > 0 then
        print('output{')
        for i, output in ipairs(node.outputs) do print(i, output) end
        print('}')
    end

    print()
end

function printNodes(definitions)
    print('---- #', definitions.get_count())
    for i, node in ipairs(definitions) do printNode(i, node) end
    print('----')
end

local definitions = plugnode.definition_manager.new()
function createDefinition(name, inputs, outputs)
    local node = definitions.create(name)
    for i, p in ipairs(inputs) do node.inputs.push_back(p) end
    for i, p in ipairs(outputs) do node.outputs.push_back(p) end
    return node
end
local def_value = createDefinition('value', {}, {{'value', 'float'}})
local def_out = createDefinition('out', {{'value', 'float'}}, {})
local def_add = createDefinition('add', {{'lhs', 'float'}, {'rhs', 'float'}},
                                 {{'value', 'float'}})
local def_mul = createDefinition('mul', {{'lhs', 'float'}, {'rhs', 'float'}},
                                 {{'value', 'float'}})
printNodes(definitions)

------------------------------------------------------------------------------
-- node scene
------------------------------------------------------------------------------
local scene = plugnode.scene.new()
function createNode(definition, position)
    return scene.create(definition, position[1], position[2])
end
function createLink(src_node, src_slot, dst_node, dst_slot)
    scene.link(src_node, src_slot, dst_node, dst_slot)
end
local node_value1 = createNode(def_value, {20, 50})
local node_value2 = createNode(def_value, {20, 150})
local node_add = createNode(def_add, {220, 70})
local node_out = createNode(def_out, {420, 90})
createLink(node_value1, 1, node_add, 1)
createLink(node_value2, 1, node_add, 2)
createLink(node_add, 1, node_out, 1)

function eval_node(node)

    local inputs = {}

    for i, slot in ipairs(node.inslots) do
        local src = slot.get_src_node()
        if src then
            local x = eval_node(src)
            if x then
                -- node to in
                slot.set_value(x)
                inputs[i] = x
            else
                inputs[i] = nil
            end
        end
    end

    local name = node.name
    if #inputs > 0 then
        -- print('inputs', table.unpack(inputs))
        if name == "add" then
            local result = inputs[1] + inputs[2]
            local target = node.outslots[1]
            target.set_value(result)
            return result
        elseif name == "out" then
            if inputs[1] then
                local target = node.outslots[1]
                -- print(target)
            end
        else
            -- not impleented
            print('not implemented', name)
        end
    else
        local outputs = {}
        for i, slot in ipairs(node.outslots) do outputs[i] = slot.value end
        -- print('outputs', table.unpack(outputs))
        return table.unpack(outputs)
    end
end

function eval_graph(scene)

    -- print("-----------")

    -- update src

    for i, node in ipairs(scene) do

        if node.is_dst then
            -- 終端ノードを処理する
            eval_node(node)
        end
    end

end

return {definitions, scene, eval_graph}
