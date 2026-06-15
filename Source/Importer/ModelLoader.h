#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Renderer/Mesh.h"

namespace ModelLoader
{
    bool LoadModel(const std::string& path, std::vector<Mesh>& outMeshes);
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
}

