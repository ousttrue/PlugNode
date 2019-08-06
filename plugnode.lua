print("create window")

function showNode(node)
    print(node.name)
    print('[input]')
    for i, input in ipairs(node.inputs) do print(i, input) end
    print('[output]')
    for i, output in ipairs(node.outputs) do print(i, output) end
end

function showNodes(node_manager)
    print('---- #', node_manager.get_count())
    for i, node in ipairs(node_manager) do
        print(i)
        showNode(node)
    end
    print('----')
end

local node_manager = plugnode.node_manager.new()
showNodes(node_manager)
local node = node_manager.add_node("int source")
node.inputs.push_back{"x", "float"}
node.inputs.push_back{"y", "float"}
node.outputs.push_back{"z", "float"}
showNodes(node_manager)

local window = plugnode.window.new()
local hwnd = window.create(640, 480, "plugnode")
if not hwnd then os.exit(1) end
print('window.create', hwnd)

local dx11 = plugnode.dx11device.new()
local device = dx11.create(hwnd)
if not device then os.exit(2) end
print('dx11.create', device)
local context = dx11.get_context()
print('dx11.get_context', context)

local scene = plugnode.dx11scene.new()
local camera = plugnode.orbit_camera.new()

local gui = plugnode.gui.new()
gui.initialize(hwnd, device, context)

local graph = plugnode.graph.new()
graph.load(node_manager[1], node_manager.get_count())

while window.is_running() do

    -- update
    local window_state = window.get_state()

    if gui.new_frame(window_state) then
        -- do nothing
    else
        camera.window_input(window_state)
    end

    -- gui.show()
    graph.show()

    local context = dx11.new_frame(window_state)

    scene.draw_teapot(context, camera.get_state())
    gui.render()

    dx11.present()

end

print("close window")
