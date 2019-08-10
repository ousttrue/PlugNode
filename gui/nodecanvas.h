#pragma once

namespace plugnode
{

class Context;
class NodeDefinitionManager;
class NodeScene;
class NodeCanvasImpl;
class NodeCanvas
{
    NodeCanvasImpl *m_impl = nullptr;

public:
    NodeCanvas();
    ~NodeCanvas();
    void Show(Context *context,
              const NodeDefinitionManager *definitions,
              NodeScene *scene);
};

} // namespace plugnode