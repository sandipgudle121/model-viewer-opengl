#include "Model.h"
#include "../Importer/ModelLoader.h"

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
}

bool Model::Load(const std::string& path)
{
    Cleanup();
    return ModelLoader::LoadModel(path, Meshes);
}

void Model::LoadCube()
{
    // Cube Data: Position (x, y, z) and Color (r, g, b)
    std::vector<float> cubeVertices = {
        // Front face (Red)
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        // Back face (Green)
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        // Top face (Blue)
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        // Bottom face (Yellow)
        -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        // Left face (Cyan)
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        // Right face (Magenta)
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f
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
    Meshes.push_back(cubeMesh);
}