#pragma once

#include <windows.h>

namespace ImGuiLayer
{
    void Init(HWND hwnd);
    void Begin();
    void End();
    void Shutdown();
}