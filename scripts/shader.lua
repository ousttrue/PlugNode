------------------------------------------------------------------------------
-- definition
------------------------------------------------------------------------------
local definitions = plugnode.definition_manager.new()
local node_definitions = {}
function createDefinition(name, inputs, outputs)
    local node = definitions.create(name)
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
    {"VERTEX_POSITION", {}, {{"float3_t", "type"}}},
    {"vec4_w1", {{"float3_t", "type"}}, {{"float4_t", "type"}}},
    {"SV_POSITION", {{"float4_t", "type"}}, {}},
    {"const_float4_color", {}, {{"value", "float4", "color"}}},
    {"SV_TARGET", {{"float4_t", "type"}}, {}}

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
function generate_vs(node)
    print("-- generate_vs --")
    local vs_context = {
        attributes = {},
        outputs = {},
        expressions = {}
    }
    -- build tree
    function traverse(node)
        for i, slot in ipairs(node.inslots) do
            if node.name == 'VERTEX_POSITION' then
                -- vertex attributes
                table.insert(vs_context.attributes, 'float3 aPosition : POSITION;\n')
            end
            if node.name == 'SV_POSITION' then
                -- vs out
                table.insert(vs_context.outputs, 'linear float4 fPosition : SV_POSITION;\n')
            end

            local src = slot.get_src_node()
            if src then
                traverse(src)
                table.insert(vs_context.expressions, string.format('%s = %s;\n', node.name, src.name))
            end
        end
    end
    traverse(node)
    
    local sb = {}
    --[[
    struct VS_INPUT
    {
        float3 m_position : POSITION;
        float3 m_normal   : NORMAL;
    };
    struct VS_OUTPUT
    {
        linear        float4 m_position     : SV_POSITION;
        linear        float3 m_normal       : NORMAL;
    };
    VS_OUTPUT main(VS_INPUT _in) 
    {
        VS_OUTPUT ret;
        ret.m_normal = mul(uWorldMatrix, float4(_in.m_normal, 0.0));
        ret.m_position = mul(uViewProjMatrix, mul(uWorldMatrix, float4(_in.m_position, 1.0)));
        return ret;
    }
    ]]

    -- attributes
    table.insert(sb, "struct VS_INPUT\n")
    table.insert(sb, "{\n")
    for i, x in ipairs(vs_context.attributes) do
        table.insert(sb, '    ')
        table.insert(sb, x)
    end
    table.insert(sb, "};\n")
    table.insert(sb, "\n");

    -- vs_out
    table.insert(sb, "struct VS_OUTPUT\n")
    table.insert(sb, "{\n")
    for i, x in ipairs(vs_context.outputs) do
        table.insert(sb, '    ')
        table.insert(sb, x)
    end
    table.insert(sb, "};\n")
    table.insert(sb, "\n");

    -- main
    table.insert(sb, "VS_OUTPUT main(VS_INPUT _in)\n")
    table.insert(sb, "{\n")
    table.insert(sb, "    VS_OUTPUT ret;\n")
    for i, x in ipairs(vs_context.expressions) do
        table.insert(sb, '    ')
        table.insert(sb, x)
    end
    table.insert(sb, "}\n")
    table.insert(sb, "");

    return table.concat(sb, "")
end
function eval_node(node)
    if node.name == "SV_POSITION" then
        local vs = generate_vs(node)
        print(vs)
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
