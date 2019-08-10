#pragma once
#include <stdint.h>
#include "mouse_state.h"

struct WindowState
{
    int Width;
    int Height;
    float ElapsedSeconds;
    float DeltaSeconds;
    MouseState Mouse;
    uint32_t Flags = 0;
};
