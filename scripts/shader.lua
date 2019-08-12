function eval_graph() end
local definitions = plugnode.definition_manager.new()
local scene = plugnode.scene.new()

return {definitions, scene, eval_graph}
