#pragma once

struct WindowState;
class DX11Context;
class GUI
{
public:
    GUI();
    ~GUI();
    void Initialize(void* hwnd, void *device, void *deviceContext);
    // return has focus
    bool NewFrame(const WindowState *state);
    void Show();
    void Render();
};
