#include "Core/Application.h"
#include "Core/Logger.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    Application app;

    if (!initializeLogger())
        return -1;

    if (!app.Initialize(hInstance, nCmdShow))
        return -1;

    app.Run();

    app.Shutdown();
    uninitializeLogger();

    return 0;
}