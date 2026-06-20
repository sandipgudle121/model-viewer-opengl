@echo off
setlocal
if not exist Build mkdir Build

set SRC=^
Source\Main.cpp ^
Source\Core\Application.cpp ^
Source\Core\Window.cpp ^
Source\Core\Logger.cpp ^
Source\Renderer\Renderer.cpp ^
Source\Renderer\Shader.cpp ^
Source\Renderer\Mesh.cpp ^
Source\Renderer\Texture.cpp ^
Source\Scene\Camera.cpp ^
Source\Scene\Model.cpp ^
Source\Importer\ModelLoader.cpp ^
Source\UI\ImGuiLayer.cpp ^
external\imgui\imgui.cpp ^
external\imgui\imgui_draw.cpp ^
external\imgui\imgui_widgets.cpp ^
external\imgui\imgui_tables.cpp ^
external\imgui\imgui_demo.cpp ^
external\imgui\backends\imgui_impl_win32.cpp ^
external\imgui\backends\imgui_impl_opengl3.cpp  

cl ^
/EHsc ^
/MD ^
/std:c++20 ^
/nologo ^
/FoBuild\ ^
/I"." ^
/I"Source" ^
/I"external\imgui" ^
/I"external\imgui\backends" ^
/I"C:\glew-2.1.0\include" ^
/I"External\assimp" ^
/Fe:app.exe ^
%SRC% ^
/link ^
/SUBSYSTEM:WINDOWS ^
user32.lib ^
gdi32.lib ^
opengl32.lib ^
/LIBPATH:"C:\glew-2.1.0\lib\Release\x64" ^
/LIBPATH:"External\assimp\assimp\lib" ^
glew32.lib ^
assimp-vc143-mt.lib

:: Copy the Assimp DLL to the root so the exe can find it at runtime
if exist "External\assimp\bin\assimp-vc143-mt.dll" copy "External\assimp\bin\assimp-vc143-mt.dll" .

app.exe
