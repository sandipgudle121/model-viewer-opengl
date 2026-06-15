#pragma once

#include <Windows.h>

class Application
{
public:
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();

    float DeltaTime = 0.0f;
    float LastFrame = 0.0f;
};