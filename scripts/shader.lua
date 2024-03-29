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
    {"VERTEX_NORMAL", "aNormal", {}, {{"float3_t", "type"}}},
    {"vec4_w1", "vec4_w1", {{"float3_t", "type"}}, {{"float4_t", "type"}}},
    {"VSOUT_POSITION", "fPosition", {{"float4_t", "type"}}, {}},
    {"CONST_float4_color", "const_color", {}, {{"value", "float4", "color"}}},
    {"PSOUT_COLOR", "result", {{"float4_t", "type"}}, {}}

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
-- vs
local vsin_pos = createNode(node_definitions.VERTEX_POSITION, {10, 20})
local vsin_normal = createNode(node_definitions.VERTEX_NORMAL, {210, 20})
local to_v4 = createNode(node_definitions.vec4_w1, {210, 100})
local vsout = createNode(node_definitions.VSOUT_POSITION, {510, 180})
createLink(vsin_pos, to_v4)
createLink(to_v4, vsout)
-- ps
local const_color = createNode(node_definitions.CONST_float4_color, {10, 260})
local psout = createNode(node_definitions.PSOUT_COLOR, {210, 340})
createLink(const_color, psout)

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
function get_semantic(name)
    local pos = string.find(name, "_")
    return string.sub(name, pos + 1)
end
function get_attribute(node)
    -- float3 aPosition: POSITION
    local type = get_type(node.definition.outputs[1])
    local name = node.name
    local semantic = get_semantic(node.definition.name)
    return string.format("%s %s : %s", type, name, semantic)
end
function get_const(node)
    -- float4 color = float4(1, 1, 1, 1)
    local type = get_type(node.definition.outputs[1])
    local name = node.name
    local value = string.format("float4(%s, %s, %s, %s)", table.unpack(node.outslots[1].value))
    return string.format("%s %s = %s", type, name, value)
end
function get_vs_output(node)
    -- linear float4 fPosition: SV_POSITION
    local type = get_type(node.definition.inputs[1])
    local name = node.name
    local semantic = get_semantic(node.definition.name)
    if semantic == "POSITION" then
        semantic = "SV_POSITION"
    end
    return string.format("%s %s : %s", type, name, semantic)
end
function get_rhs(node)
    local defname = node.definition.name
    local pos = string.find(defname, "_")
    print(pos, string.sub(defname, 1, pos - 1))
    if string.sub(defname, 1, pos - 1) == "VERTEX" then
        return "_in." .. node.name
    else
        return node.name
    end
end
function get_expression(lhs, rhs)
    if lhs.definition.name == "VSOUT_POSITION" then
        return string.format("_out.%s = %s", lhs.name, get_rhs(rhs))
    end
    if lhs.definition.name == "vec4_w1" then
        return string.format("%s %s = float4(%s, 1)", get_type(lhs.definition.outputs[1]), lhs.name, get_rhs(rhs))
    end
    if lhs.definition.name == "PSOUT_COLOR" then
        return string.format("return %s", get_rhs(rhs))
    end
    return string.format("unknown expression: %s = %s", lhs, rhs)
end
function create_context()
    local self = {
        expressions = {},
        nodes = {}
    }
    self.add_node = function(node)
        table.insert(self.nodes, node)
    end
    self.write_attributes = function(sb)
        table.insert(sb, "struct VS_INPUT\n")
        table.insert(sb, "{\n")
        for i, x in ipairs(self.nodes) do
            if string.sub(x.definition.name, 1, 7) == "VERTEX_" then
                table.insert(sb, string.format("    %s;\n", get_attribute(x)))
            end
        end
        table.insert(sb, "};\n")
        table.insert(sb, "\n")
    end
    self.write_const = function(sb)
        for i, x in ipairs(self.nodes) do
            if string.sub(x.definition.name, 1, 6) == "CONST_" then
                table.insert(sb, string.format("    %s;\n", get_const(x)))
            end
        end
    end
    self.write_vsout = function(sb)
        table.insert(sb, "struct VS_OUTPUT\n")
        table.insert(sb, "{\n")
        for i, x in ipairs(self.nodes) do
            if string.sub(x.definition.name, 1, 6) == "VSOUT_" then
                table.insert(sb, string.format("    %s;\n", get_vs_output(x)))
            end
        end
        table.insert(sb, "};\n")
        table.insert(sb, "\n")
    end
    self.add_expression = function(lhs, rhs)
        table.insert(self.expressions, {lhs, rhs})
    end
    self.write_expressions = function(sb)
        -- main
        for i, x in ipairs(self.expressions) do
            local lhs = x[1]
            local rhs = x[2]
            table.insert(sb, string.format("    %s;\n", get_expression(lhs, rhs)))
        end
    end

    self.to_vs = function(sb)
        local sb = {}
        self.write_attributes(sb)
        self.write_vsout(sb)
        -- main
        table.insert(sb, "VS_OUTPUT main(VS_INPUT _in)\n")
        table.insert(sb, "{\n")
        self.write_const(sb)
        table.insert(sb, string.format("    VS_OUTPUT _out;\n", vsout))
        self.write_expressions(sb, vsout)
        table.insert(sb, "    return _out;\n")
        table.insert(sb, "}\n")
        table.insert(sb, "")
        return table.concat(sb, "")
    end

    self.to_ps = function(sb)
        local sb = {}
        self.write_vsout(sb)
        -- main

        table.insert(sb, "float4 main(VS_OUTPUT _in): SV_Target\n")
        table.insert(sb, "{\n")
        self.write_const(sb)
        self.write_expressions(sb)
        table.insert(sb, "}\n")
        table.insert(sb, "")
        return table.concat(sb, "")
    end

    return self
end
function generate_vs(node)
    print("-- generate_vs --")
    local context = create_context()
    -- build tree
    function traverse(node)
        context.add_node(node)
        for i, slot in ipairs(node.inslots) do
            local src = slot.get_src_node()
            if src then
                traverse(src)
                context.add_expression(node, src)
            end
        end
    end
    traverse(node)
    -- export
    table.insert(context.nodes, vsin_normal)
    return context.to_vs()
end
function generate_ps(node)
    print("-- generate_ps --")
    local context = create_context()
    -- build tree
    function traverse(node)
        context.add_node(node)
        for i, slot in ipairs(node.inslots) do
            local src = slot.get_src_node()
            if src then
                traverse(src)
                context.add_expression(node, src)
            end
        end
    end
    traverse(node)
    -- export
    return context.to_ps()
end
function eval_node(node)
    if node.definition.name == "VSOUT_POSITION" then
        return "vs", generate_vs(node)
    elseif node.definition.name == "PSOUT_COLOR" then
        return "ps", generate_ps(node)
    end
end
function eval_graph(scene)
    local t = {}
    for i, node in ipairs(scene) do
        if node.is_dst then
            -- 終端ノードを処理する
            local kind, shader = eval_node(node)
            if kind == "vs" then
                t.vs = shader
            elseif kind == "ps" then
                t.ps = shader
            end
        end
    end
    return t.vs, t.ps
end

return {definitions, scene, eval_graph}
