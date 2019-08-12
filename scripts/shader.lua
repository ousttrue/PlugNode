function eval_graph()
end

------------------------------------------------------------------------------
-- definition
------------------------------------------------------------------------------
local definitions = plugnode.definition_manager.new()
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
        createDefinition(table.unpack(d))
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

------------------------------------------------------------------------------
return {definitions, scene, eval_graph}
