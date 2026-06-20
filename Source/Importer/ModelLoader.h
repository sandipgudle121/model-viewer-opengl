#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Renderer/Mesh.h"

struct ModelInfo;

namespace ModelLoader
{
    bool LoadModel(const std::string& path, std::vector<Mesh>& outMeshes, ModelInfo& outInfo);
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes, ModelInfo& outInfo);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelInfo& outInfo);
}
