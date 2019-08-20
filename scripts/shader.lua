------------------------------------------------------------------------------
-- definition
------------------------------------------------------------------------------
local definitions = plugnode.definition_manager.new()
local node_definitions = {}
function createDefinition(name, default_name, inputs, outputs)
    local node = definitions.create(name, default_name)
    for i, p in ipairs(inputs) do
        node.inputs.push_back(p)
    end
    for i, p in ipairs(outputs) do
        node.outputs.push_back(p)
    end
    return node
end
function define(srcs)
    for i, d in ipairs(srcs) do
        node_definitions[d[1]] = createDefinition(table.unpack(d))
    end
end

--[[
cbuffer cbContextData : register(b0)
{
    float4x4 uWorldMatrix;
    float4x4 uViewProjMatrix;
};

struct VS_INPUT
{
    float3 m_position : POSITION;
    float3 m_normal   : NORMAL;
};

VS_OUTPUT main(VS_INPUT _in) 
{
    VS_OUTPUT ret;
    ret.m_normal = mul(uWorldMatrix, float4(_in.m_normal, 0.0));
    ret.m_position = mul(uViewProjMatrix, mul(uWorldMatrix, float4(_in.m_position, 1.0)));
    return ret;
}

struct VS_OUTPUT
{
    linear        float4 m_position     : SV_POSITION;
    linear        float3 m_normal       : NORMAL;
};

float4 main(VS_OUTPUT _in): SV_Target
{
    float3 nrm = normalize(_in.m_normal);
    float3 ldir = normalize(float3(1.0, 1.0, 1.0));
    float ndotl = max(dot(nrm, ldir), 0.0) * 0.5;
    return float4(ndotl, ndotl, ndotl, 1.0);
}
--]]
define {
    {"VERTEX_POSITION", "aPosition", {}, {{"float3_t", "type"}}},
    {"vec4_w1", "vec4_w1", {{"float3_t", "type"}}, {{"float4_t", "type"}}},
    {"SV_POSITION", "fPosition", {{"float4_t", "type"}}, {}},
    {"const_float4_color", "const_color", {}, {{"value", "float4", "color"}}},
    {"SV_TARGET", "result", {{"float4_t", "type"}}, {}}

    -- {"uniform_matrix", {}, {{"value", "mat4_t"}}}
    -- {"attr_position_vec3", {}, {{"vec3", "type"}}},
    -- {"attr_normal_vec3", {}, {{"vec3", "type"}}},
    -- {"vec3_to_vec4", {{"vec3", "type", "w", "float"}}, {"vec4", "type"}},
    -- {"transform", {{"lhs", "mat4"}, {"vec4", "type"}}, {"vec4", "type"}},
    -- {"normalize", {{"value", "vec3"}}, {{"value", "vec3"}}},
    -- {"uniform_vec3", {}, {{"value", "vec3"}}},
    -- {"vs_position", {{"value", "vec3"}}, {{"value", "vec3"}}},
    -- {"vs_normal", {{"value", "vec3"}}, {{"value", "vec3"}}},
    -- {"dot", {{"value", "vec3"}, {"value", "vec3"}}, {{"value", "float"}}},
    -- {"max", {{"value", "float"}, {"value", "float"}}, {{"value", "float"}}},
    -- {"mul", {{"value", "float"}, {"value", "float"}}, {{"value", "float"}}},
    -- {
    --     "vec3",
    --     {{"x", "float"}, {"y", "float"}, {"z", "float"}},
    --     {{"value", "vec3"}}
    -- },
    -- {
    --     "vec4",
    --     {{"x", "float"}, {"y", "float"}, {"z", "float"}, {"w", "float"}},
    --     {{"value", "vec4"}}
    -- },
    -- {"shader_out", {{"value", "vec4"}}, {}}
}

------------------------------------------------------------------------------
-- scene
------------------------------------------------------------------------------
local scene = plugnode.scene.new()
function createNode(definition, position)
    return scene.create(definition, position[1], position[2])
end
function unpack_slot(src)
    if type(src) == "table" then
        return table.unpack(src)
    else
        return src, 1
    end
end
function createLink(src, dst)
    local src_node, src_slot = unpack_slot(src)
    local dst_node, dst_slot = unpack_slot(dst)
    scene.link(src_node, src_slot, dst_node, dst_slot)
end
local v_pos = createNode(node_definitions.VERTEX_POSITION, {10, 20})
local to_v4 = createNode(node_definitions.vec4_w1, {210, 100})
local sv_pos = createNode(node_definitions.SV_POSITION, {510, 180})
createLink(v_pos, to_v4)
createLink(to_v4, sv_pos)

------------------------------------------------------------------------------
-- eval
------------------------------------------------------------------------------
function get_type(definition)
    if definition.type == "type" then
        return string.sub(definition.name, 1, -3)
    else
        return definition.type
    end
end
function get_attribute(node)
    -- float3 aPosition: POSITION
    local type = get_type(node.definition.outputs[1])
    local name = node.name
    local semantic = string.sub(node.definition.name, 8)
    return string.format("%s %s : %s", type, name, semantic)
end
function get_vs_output(node)
    -- linear float4 fPosition: SV_POSITION
    local type = get_type(node.definition.inputs[1])
    local name = node.name
    local semantic = node.definition.name
    return string.format("%s %s : %s", type, name, semantic)
end
function get_expression(lhs, rhs)
    if lhs.definition.name == "SV_POSITION" then
        return string.format("return %s", rhs.name)
    end
    if lhs.definition.name == "vec4_w1" then
        return string.format("%s %s=float4(%s, 1)", get_type(lhs.definition.outputs[1]), lhs.name, rhs.name)
    end
    return string.format("unknown expression: %s = %s", lhs, rhs)
end
function generate_vs(node)
    print("-- generate_vs --")
    local vs_context = {
        attributes = {},
        outputs = {},
        expressions = {},
        nodes = {}
    }
    vs_context.add_node = function(node)
        if node.definition.name == "VERTEX_POSITION" then
            table.insert(vs_context.attributes, node)
        elseif node.definition.name == "SV_POSITION" then
            table.insert(vs_context.outputs, node)
        end
    end
    vs_context.write_attributes = function(sb)
        table.insert(sb, "struct VS_INPUT\n")
        table.insert(sb, "{\n")
        for i, x in ipairs(vs_context.attributes) do
            table.insert(sb, string.format("    %s\n", get_attribute(x)))
        end
        table.insert(sb, "};\n")
        table.insert(sb, "\n")
    end
    vs_context.write_out = function(sb)
        table.insert(sb, "struct VS_OUTPUT\n")
        table.insert(sb, "{\n")
        for i, x in ipairs(vs_context.outputs) do
            table.insert(sb, string.format("    %s\n", get_vs_output(x)))
        end
        table.insert(sb, "};\n")
        table.insert(sb, "\n")
    end
    vs_context.add_expression = function(lhs, rhs)
        table.insert(vs_context.expressions, {lhs, rhs})
    end
    vs_context.write_expressions = function(sb)
        -- main
        table.insert(sb, "VS_OUTPUT main(VS_INPUT _in)\n")
        table.insert(sb, "{\n")
        table.insert(sb, "    VS_OUTPUT ret;\n")
        for i, x in ipairs(vs_context.expressions) do
            local lhs = x[1]
            local rhs = x[2]
            table.insert(sb, string.format("    %s;\n", get_expression(lhs, rhs)))
        end
        table.insert(sb, "}\n")
        table.insert(sb, "")
    end

    -- build tree
    function traverse(node)
        vs_context.add_node(node)
        for i, slot in ipairs(node.inslots) do
            local src = slot.get_src_node()
            if src then
                traverse(src)
                vs_context.add_expression(node, src)
            end
        end
    end
    traverse(node)

    local sb = {}
    vs_context.write_attributes(sb)
    vs_context.write_out(sb)
    vs_context.write_expressions(sb)

    return table.concat(sb, "")
end
function eval_node(node)
    if node.definition.name == "SV_POSITION" then
        local vs = generate_vs(node)
        print(vs)
    elseif node.name == "SV_TARGET" then
        local ps = generate_ps(node)
        print(ps)
    end
end
function eval_graph(scene)
    for i, node in ipairs(scene) do
        if node.is_dst then
            -- 終端ノードを処理する
            eval_node(node)
        end
    end
end

return {definitions, scene, eval_graph}
