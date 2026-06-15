#include "ModelLoader.h"
#include "../Core/Logger.h"

bool ModelLoader::LoadModel(const std::string& path, std::vector<Mesh>& outMeshes)
{
    if (gpfile) fprintf(gpfile, "ModelLoader: Attempting to load model from: %s\n", path.c_str());

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        if (gpfile) fprintf(gpfile, "ModelLoader: ASSIMP ERROR: %s\n", importer.GetErrorString());
        return false;
    }

    ProcessNode(scene->mRootNode, scene, outMeshes);
    return true;
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        outMeshes.emplace_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, outMeshes);
    }
}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Optimization: Reserve memory upfront to avoid reallocations
    vertices.reserve(mesh->mNumVertices * 6); // 3 for Pos, 3 for Color
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // Position
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Color (Fall back to white if no vertex colors exist)
        if (mesh->HasVertexColors(0))
        {
            vertices.push_back(mesh->mColors[0][i].r);
            vertices.push_back(mesh->mColors[0][i].g);
            vertices.push_back(mesh->mColors[0][i].b);
        }
        else
        {
            vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh m;
    m.Create(vertices, indices);
    return m;
}