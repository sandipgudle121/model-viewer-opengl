#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<gl/glew.h>     /* this must be before <gl/gl.h> */
#include<gl/GL.h>

#include "Application.h"
#include "../Renderer/Renderer.h"
#include "imgui.h"
#include "../UI/ImGuiLayer.h"
#include "Logger.h"
using namespace vmath;

// MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Link With OpenGL Library
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"assimp-vc143-mt.lib")

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND ,UINT , WPARAM, LPARAM);
void ToggleFullscreen(void);

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global Variable Declaration for OpenGL 
HDC ghdc = NULL;
HGLRC ghrc = NULL;      

// Global Variable For Fullscreen of Window
HWND ghwnd = NULL;      
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT)};
BOOL gbFullScreen = FALSE;
int iResult = 0;
BOOL gbActive = FALSE;
DWORD gLastMouseMoveTime = 0;
BOOL gCursorVisible = TRUE;

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wndclass;
    TCHAR szAppName[] = TEXT("MyWindow");

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_BIG));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_BIG));

    if (!RegisterClassEx(&wndclass))
        return false;

    ghwnd = CreateWindowEx(WS_EX_APPWINDOW,
        szAppName,
        TEXT("Model Viewer Tool"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        100, 100, WIN_WIDTH, WIN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!ghwnd)
        return false;

    ghdc = GetDC(ghwnd);
    iResult = initialize(ghwnd);
    if (iResult != 0) return false;

    resize(WIN_WIDTH, WIN_HEIGHT);

    gLastMouseMoveTime = GetTickCount();
    ImGuiLayer::Init(ghwnd);

    ShowWindow(ghwnd, nCmdShow);
    SetForegroundWindow(ghwnd);
    SetFocus(ghwnd);

    ToggleFullscreen();
    gbFullScreen = TRUE;

    LastFrame = (float)GetTickCount64() * 0.001f;

    return true;
}

void Application::Run()
{
    MSG msg;
    bool bDone = false;

    while (!bDone)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                bDone = true;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (gbActive)
            {
                float current = (float)GetTickCount64() * 0.001f;
                DeltaTime = current - LastFrame;
                LastFrame = current;

                // Cursor auto-hide logic (3 second timeout)
                if (gCursorVisible && (GetTickCount() - gLastMouseMoveTime > 3000))
                {
                    ShowCursor(FALSE);
                    gCursorVisible = FALSE;
                }

                update(DeltaTime);
                display(ghdc);
            }
        }
    }
}

void Application::Shutdown()
{
    ImGuiLayer::Shutdown();
    uninitialize();
    if (ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam, LPARAM lParam )
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, iMsg, wParam, lParam))
        return true;

    // Code 
    switch(iMsg)
    {
        case WM_SETFOCUS :
            gbActive = TRUE;
            gLastMouseMoveTime = GetTickCount();
        break;

        case WM_KILLFOCUS :
            gbActive = FALSE;
        break;

        case WM_MOUSEMOVE:
            gLastMouseMoveTime = GetTickCount();
            if (gCursorVisible == FALSE)
            {
                ShowCursor(TRUE);
                gCursorVisible = TRUE;
            }
        break;

        case WM_MOUSEWHEEL:
            if (!ImGui::GetIO().WantCaptureMouse)
                handleMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;

        case WM_SIZE :
            resize(LOWORD(lParam),HIWORD(lParam));
        break;

        case WM_ERASEBKGND :
            return(0);

        case WM_KEYDOWN :
            switch(LOWORD(wParam))
            {
                case VK_ESCAPE :
                    DestroyWindow(hwnd);
                    break;
            }
        break;

        case WM_CHAR :
            switch(LOWORD(wParam))
            {
                case 'F':
                case 'f':
                    if(gbFullScreen ==FALSE)
                    {
                        ToggleFullscreen();
                        gbFullScreen= TRUE;
                    }
                    else
                    {
                        ToggleFullscreen();
                        gbFullScreen = FALSE;
                    }
                break;
            }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default :
            break;
    }
    return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void ToggleFullscreen(void)
{
    // Local Variable Declarations
    MONITORINFO mi = { sizeof(MONITORINFO)};

    // Code
    if(gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
            {
                SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right - mi.rcMonitor.left,mi.rcMonitor.bottom - mi.rcMonitor.top,SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
    }
    else
    {
        SetWindowPlacement(ghwnd,&wpPrev);
        SetWindowLong(ghwnd,GWL_STYLE ,
        dwStyle | WS_OVERLAPPEDWINDOW );
        SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE  | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}
