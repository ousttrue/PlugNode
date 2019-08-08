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
function createNode(name, inputs, outputs)
    local node = definitions.create(name)
    for i, p in ipairs(inputs) do node.inputs.push_back(p) end
    for i, p in ipairs(outputs) do node.outputs.push_back(p) end
end
createNode('value', {}, {{'value', 'float'}})
createNode('out', {{'value', 'float'}}, {})
createNode('add', {{'lhs', 'float'}, {'lhs', 'float'}}, {{'value', 'float'}})
createNode('mul', {{'lhs', 'float'}, {'lhs', 'float'}}, {{'value', 'float'}})
printNodes(definitions)

------------------------------------------------------------------------------
-- node scene
------------------------------------------------------------------------------
local scene = plugnode.scene.new()

------------------------------------------------------------------------------
-- gui
------------------------------------------------------------------------------
local window = app.window.new()
local hwnd = window.create(640, 480, "plugnode")
if not hwnd then os.exit(1) end
print('window.create', hwnd)

local dx11 = app.dx11device.new()
local device = dx11.create(hwnd)
if not device then os.exit(2) end
print('dx11.create', device)
local context = dx11.get_context()
print('dx11.get_context', context)

local teapot = app.dx11scene.new()
local camera = app.orbit_camera.new()

local gui = app.gui.new()
gui.initialize(hwnd, device, context)

local graph = plugnode.graph.new()

while window.is_running() do

    -- update
    local window_state = window.get_state()

    if gui.new_frame(window_state) then
        -- do nothing
    else
        camera.window_input(window_state)
    end

    -- gui.show()
    graph.imgui(definitions, scene)

    local context = dx11.new_frame(window_state)

    teapot.draw_teapot(context, camera.get_state())
    gui.render()

    dx11.present()

end

print("close window")
