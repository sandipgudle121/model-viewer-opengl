#include "Renderer.h"
#include "../Core/Logger.h"
#include "../Scene/Camera.h"
#include "../Scene/Model.h"
#include "../UI/ImGuiLayer.h"
#include "imgui.h"
#include "Mesh.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/* OpenGL related variables */
GLuint shaderProgramObject = 0;
enum
{
    SSG_ATTRIBUTE_POSITION = 0,
    SSG_ATTRIBUTE_NORMAL,
    SSG_ATTRIBUTE_TEXCOORD,
    SSG_ATTRIBUTE_COLOR
};

Model gCubeModel;
GLuint mvpMatrixUniform = 0;
vmath::mat4 perspectiveProjectionMatrix;

Camera gCamera;
bool gbWireframe = false;
float gModelYaw = 0.0f;
float gModelPitch = 0.0f;

void printGLInfo(void)
{
    GLint numExtensions;
    fprintf(gpfile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    fprintf(gpfile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    fprintf(gpfile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
    fprintf(gpfile, "OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

std::string loadShaderSource(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        if (gpfile) fprintf(gpfile, "ERROR: Failed to open shader file: %s\n", filename);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FormatCount(std::uint64_t value)
{
    std::string text = std::to_string(value);
    for (int insertPosition = (int)text.length() - 3; insertPosition > 0; insertPosition -= 3)
    {
        text.insert((size_t)insertPosition, ",");
    }

    return text;
}

const char* YesNo(bool value)
{
    return value ? "YES" : "NO";
}

void DrawModelInformationPanel(const ModelInfo& info)
{
    ImGui::Begin("Model Information");
    ImGui::Text("Meshes: %s", FormatCount(info.MeshCount).c_str());
    ImGui::Text("Vertices: %s", FormatCount(info.VertexCount).c_str());
    ImGui::Text("Triangles: %s", FormatCount(info.TriangleCount).c_str());
    ImGui::Text("Indices: %s", FormatCount(info.IndexCount).c_str());
    ImGui::Text("Materials: %s", FormatCount(info.MaterialCount).c_str());
    ImGui::Text("Textures: %s", FormatCount(info.TextureCount).c_str());
    ImGui::Text("Import Time: %.2f ms", info.ImportTimeMs);

    ImGui::Separator();
    ImGui::Text("Has Normals: %s", YesNo(info.HasNormals));
    ImGui::Text("Has UVs: %s", YesNo(info.HasTexCoords));
    ImGui::Text("Has Vertex Colors: %s", YesNo(info.HasVertexColors));

    ImGui::Separator();
    ImGui::Text("Path:");
    ImGui::TextWrapped("%s", info.Path.empty() ? "N/A" : info.Path.c_str());

    ImGui::Separator();
    ImGui::Text("Bounds:");
    if (info.HasBounds)
    {
        ImGui::Text("Min(%.3f, %.3f, %.3f)", info.BoundsMin[0], info.BoundsMin[1], info.BoundsMin[2]);
        ImGui::Text("Max(%.3f, %.3f, %.3f)", info.BoundsMax[0], info.BoundsMax[1], info.BoundsMax[2]);
    }
    else
    {
        ImGui::Text("Min(N/A)");
        ImGui::Text("Max(N/A)");
    }

    ImGui::Separator();
    ImGui::Text("Loaded: %s", YesNo(info.Loaded));
    ImGui::End();
}

int initialize(HWND hwnd)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    HDC hdc = GetDC(hwnd);

    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER);
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;

    iPixelFormatIndex = ChoosePixelFormat(hdc, &pfd);
    if (iPixelFormatIndex == 0) return -2;
    if (SetPixelFormat(hdc, iPixelFormatIndex, &pfd) == FALSE) return -3;

    HGLRC hrc = wglCreateContext(hdc);
    if (hrc == NULL) return -4;
    if (wglMakeCurrent(hdc, hrc) == FALSE) return -5;

    if (glewInit() != GLEW_OK) return -6;

    printGLInfo();

    /* Shaders */
    std::string vertexShaderSource = loadShaderSource("Source/Renderer/vertexShader.glsl");
    const GLchar* vertexShaderSourceCode = vertexShaderSource.c_str();

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint status;
    char infoLog[512];
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(vertexShaderObject, 512, NULL, infoLog);
        if (gpfile) fprintf(gpfile, "Vertex Shader Compilation Error: %s\n", infoLog);
        return -7;
    }

    std::string fragmentShaderSource = loadShaderSource("Source/Renderer/fragmentShader.glsl");
    const GLchar* fragmentShaderSourceCode = fragmentShaderSource.c_str();

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    glCompileShader(fragmentShaderObject);

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(fragmentShaderObject, 512, NULL, infoLog);
        if (gpfile) fprintf(gpfile, "Fragment Shader Compilation Error: %s\n", infoLog);
        return -8;
    }

    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_NORMAL, "aNormal");
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_TEXCOORD, "aTexCoord");
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_COLOR, "aColor");
    glLinkProgram(shaderProgramObject);

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgramObject, 512, NULL, infoLog);
        if (gpfile) fprintf(gpfile, "Shader Program Linking Error: %s\n", infoLog);
        return -9;
    }

    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

    // Try to load an external model, fallback to cube if it fails
    if (!gCubeModel.Load("Assets/Models/fatalis/source/fatalis.fbx")) {
        gCubeModel.LoadCube();
    }

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    gCamera.Target = vmath::vec3(0.0f, 0.0f, 0.0f);
    gCamera.Radius = 6.0f;
    gCamera.Yaw = 0.0f;
    gCamera.Pitch = 0.0f;
    gCamera.Up = vmath::vec3(0.0f, 1.0f, 0.0f);
    gModelYaw = 0.0f;
    gModelPitch = 0.0f;

    perspectiveProjectionMatrix = vmath::mat4::identity();
    return 0;
}

void resize(int width, int height)
{
    if (height <= 0) height = 1;
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(HDC hdc)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gbWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ImGuiLayer::Begin();
    {
        ImGui::Begin("Toolbox");
        ImGui::Checkbox("Wireframe Mode", &gbWireframe);
        ImGui::End();

        DrawModelInformationPanel(gCubeModel.GetInfo());
    }

    glUseProgram(shaderProgramObject);
    
    // 1. Model Matrix: Rotate the model directly for unrestricted viewer-style tumble
    vmath::mat4 modelMatrix =
        vmath::rotate(gModelYaw, 0.0f, 1.0f, 0.0f) *
        vmath::rotate(gModelPitch, 1.0f, 0.0f, 0.0f);
    
    // 2. View Matrix: Using our new Camera class
    vmath::mat4 viewMatrix = gCamera.GetViewMatrix();
    
    // 3. MVP Matrix: P * V * M
    vmath::mat4 mvpMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix);
    
    gCubeModel.Draw();
    
    glUseProgram(0);

    ImGuiLayer::End();

    SwapBuffers(hdc);
}

void update(float deltaTime)
{
    // Model viewer input handling
    static POINT lastMousePos = { 0, 0 };
    static bool isDragging = false;
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);

    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
    {
        bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
        if (mouseDown)
        {
            if (!isDragging)
            {
                isDragging = true;
                lastMousePos = currentMousePos;
                return; // Skip first frame to avoid delta jump
            }

            int dx = currentMousePos.x - lastMousePos.x;
            int dy = currentMousePos.y - lastMousePos.y;
            lastMousePos = currentMousePos;

            // Left Mouse: Rotate model
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
            {
                gModelYaw += (float)dx * 0.2f;
                gModelPitch += (float)dy * 0.2f;
            }

            // Middle Mouse: Pan
            if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                // Convert angles to radians for vector math
                float p = vmath::radians(gCamera.Pitch);
                float y = vmath::radians(gCamera.Yaw);
                float cp = cosf(p);

                // Calculate local right and up vectors relative to camera view
                vmath::vec3 forward = vmath::normalize(vmath::vec3(-cp * sinf(y), -sinf(p), -cp * cosf(y)));
                vmath::vec3 right = vmath::normalize(vmath::cross(forward, gCamera.Up));
                vmath::vec3 actualUp = vmath::normalize(vmath::cross(right, forward));

                // Pan speed scales with distance to target
                float panSpeed = gCamera.Radius * 0.0015f;
                gCamera.Target += right * (float)-dx * panSpeed;
                gCamera.Target += actualUp * (float)dy * panSpeed;
            }
        }
        else
        {
            isDragging = false;
        }
    }

    // Wireframe toggle logic using 'X' key
    static bool xPressed = false;
    if (GetAsyncKeyState('X') & 0x8000) {
        if (!xPressed) {
            gbWireframe = !gbWireframe;
            xPressed = true;
        }
    } else {
        xPressed = false;
    }
}

void handleMouseWheel(short delta)
{
    float zoomSpeed = gCamera.Radius * 0.1f;
    if (delta > 0) gCamera.Radius -= zoomSpeed;
    else gCamera.Radius += zoomSpeed;

    if (gCamera.Radius < 0.1f) gCamera.Radius = 0.1f;
}

void uninitialize(void)
{
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject);
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
            if (pShaders)
            {
                glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                }
                free(pShaders);
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    gCubeModel.Cleanup();

    HGLRC ghrc = wglGetCurrentContext();
    if (ghrc)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(ghrc);
    }
}
