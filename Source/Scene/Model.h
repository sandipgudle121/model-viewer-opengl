#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Renderer/Mesh.h"
#include "../vmath.h"

struct ModelInfo
{
    bool Loaded = false;
    bool HasBounds = false;
    bool HasNormals = false;
    bool HasTexCoords = false;
    bool HasVertexColors = false;
    bool HasAlbedoTexture = false;
    bool HasNodeTransforms = false;

    std::uint64_t MeshCount = 0;
    std::uint64_t VertexCount = 0;
    std::uint64_t TriangleCount = 0;
    std::uint64_t IndexCount = 0;
    std::uint64_t MaterialCount = 0;
    std::uint64_t TextureCount = 0;
    double ImportTimeMs = 0.0;
    std::string Path;

    vmath::vec3 BoundsMin = vmath::vec3(0.0f, 0.0f, 0.0f);
    vmath::vec3 BoundsMax = vmath::vec3(0.0f, 0.0f, 0.0f);
    vmath::vec3 BoundsCenter = vmath::vec3(0.0f, 0.0f, 0.0f);
    float BoundsRadius = 0.0f;
    float NormalizedScale = 1.0f;
};

class Model
{
public:
    void Draw();
    void Cleanup();
    bool Load(const std::string& path);
    void LoadCube(); // Hardcoded cube mesh generator
    const ModelInfo& GetInfo() const;

private:
    std::vector<Mesh> Meshes;
    ModelInfo Info;
};
