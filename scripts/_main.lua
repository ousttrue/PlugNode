definitions, scene, eval_graph = table.unpack(require(arg[1]))

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

    gui.show()

    if graph.imgui(definitions, scene) then
        -- graph updated
        local vs, ps = eval_graph(scene)
        if vs and ps then
            print(vs, ps)
            teapot.set_shader(vs, ps)
        end
    end

    local context = dx11.new_frame(window_state)

    teapot.draw_teapot(context, camera.get_state())
    gui.render()

    dx11.present()

end

print("close window")
