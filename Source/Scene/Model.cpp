#include "Model.h"
#include "../Importer/ModelLoader.h"
#include <utility>

void Model::Draw()
{
    for (Mesh& mesh : Meshes)
    {
        mesh.Draw();
    }
}

void Model::Cleanup()
{
    for (Mesh& mesh : Meshes)
    {
        mesh.Cleanup();
    }
    Meshes.clear();
    Info = ModelInfo();
}

bool Model::Load(const std::string& path)
{
    Cleanup();
    bool loaded = ModelLoader::LoadModel(path, Meshes, Info);
    Info.Loaded = loaded;
    return loaded;
}

void Model::LoadCube()
{
    Cleanup();

    // Cube Data: Position, Normal, TexCoord, Color
    std::vector<Vertex> cubeVertices = {
        // Front face (Red)
        { vmath::vec3(-1.0f,  1.0f,  1.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
        { vmath::vec3(-1.0f, -1.0f,  1.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f,  1.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f,  1.0f,  1.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
        // Back face (Green)
        { vmath::vec3(-1.0f,  1.0f, -1.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3(-1.0f, -1.0f, -1.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f, -1.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f,  1.0f, -1.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
        // Top face (Blue)
        { vmath::vec3(-1.0f,  1.0f, -1.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3(-1.0f,  1.0f,  1.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3( 1.0f,  1.0f,  1.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3( 1.0f,  1.0f, -1.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
        // Bottom face (Yellow)
        { vmath::vec3(-1.0f, -1.0f, -1.0f), vmath::vec3(0.0f, -1.0f, 0.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3(-1.0f, -1.0f,  1.0f), vmath::vec3(0.0f, -1.0f, 0.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f,  1.0f), vmath::vec3(0.0f, -1.0f, 0.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f, -1.0f), vmath::vec3(0.0f, -1.0f, 0.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f) },
        // Left face (Cyan)
        { vmath::vec3(-1.0f,  1.0f, -1.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(0.0f, 1.0f, 1.0f, 1.0f) },
        { vmath::vec3(-1.0f, -1.0f, -1.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(0.0f, 1.0f, 1.0f, 1.0f) },
        { vmath::vec3(-1.0f, -1.0f,  1.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(0.0f, 1.0f, 1.0f, 1.0f) },
        { vmath::vec3(-1.0f,  1.0f,  1.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(0.0f, 1.0f, 1.0f, 1.0f) },
        // Right face (Magenta)
        { vmath::vec3( 1.0f,  1.0f, -1.0f), vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 1.0f), vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f, -1.0f), vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 0.0f), vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3( 1.0f, -1.0f,  1.0f), vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 0.0f), vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f) },
        { vmath::vec3( 1.0f,  1.0f,  1.0f), vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 1.0f), vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f) }
    };

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2,  0, 2, 3,       // Front
        4, 5, 6,  4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,     // Top
        12, 13, 14, 12, 14, 15,  // Bottom
        16, 17, 18, 16, 18, 19,  // Left
        20, 21, 22, 20, 22, 23   // Right
    };

    Mesh cubeMesh;
    cubeMesh.Create(cubeVertices, cubeIndices);
    Meshes.push_back(std::move(cubeMesh));

    Info.Loaded = true;
    Info.HasBounds = true;
    Info.HasNormals = true;
    Info.HasTexCoords = true;
    Info.HasVertexColors = true;
    Info.MeshCount = 1;
    Info.VertexCount = cubeVertices.size();
    Info.TriangleCount = cubeIndices.size() / 3;
    Info.IndexCount = cubeIndices.size();
    Info.MaterialCount = 0;
    Info.TextureCount = 0;
    Info.ImportTimeMs = 0.0;
    Info.Path = "Generated Cube";
    Info.BoundsMin = vmath::vec3(-1.0f, -1.0f, -1.0f);
    Info.BoundsMax = vmath::vec3(1.0f, 1.0f, 1.0f);
}

const ModelInfo& Model::GetInfo() const
{
    return Info;
}
