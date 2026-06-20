#include "ModelLoader.h"
#include "../Core/Logger.h"
#include "../Scene/Model.h"
#include <chrono>
#include <cstdint>

namespace
{
    std::uint64_t CountMaterialTextures(const aiScene* scene)
    {
        std::uint64_t textureCount = 0;

        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            aiMaterial* material = scene->mMaterials[i];
            for (int type = aiTextureType_DIFFUSE; type <= AI_TEXTURE_TYPE_MAX; type++)
            {
                textureCount += material->GetTextureCount(static_cast<aiTextureType>(type));
            }
        }

        return textureCount;
    }

    void ExpandBounds(ModelInfo& info, const aiVector3D& position)
    {
        if (!info.HasBounds)
        {
            info.BoundsMin = vmath::vec3(position.x, position.y, position.z);
            info.BoundsMax = vmath::vec3(position.x, position.y, position.z);
            info.HasBounds = true;
            return;
        }

        if (position.x < info.BoundsMin[0]) info.BoundsMin[0] = position.x;
        if (position.y < info.BoundsMin[1]) info.BoundsMin[1] = position.y;
        if (position.z < info.BoundsMin[2]) info.BoundsMin[2] = position.z;

        if (position.x > info.BoundsMax[0]) info.BoundsMax[0] = position.x;
        if (position.y > info.BoundsMax[1]) info.BoundsMax[1] = position.y;
        if (position.z > info.BoundsMax[2]) info.BoundsMax[2] = position.z;
    }
}

bool ModelLoader::LoadModel(const std::string& path, std::vector<Mesh>& outMeshes, ModelInfo& outInfo)
{
    if (gpfile) fprintf(gpfile, "ModelLoader: Attempting to load model from: %s\n", path.c_str());

    outInfo = ModelInfo();
    outInfo.Path = path;

    Assimp::Importer importer;
    auto start = std::chrono::high_resolution_clock::now();
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
    auto end = std::chrono::high_resolution_clock::now();
    outInfo.ImportTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        if (gpfile) fprintf(gpfile, "ModelLoader: ASSIMP ERROR: %s\n", importer.GetErrorString());
        return false;
    }

    outInfo.MaterialCount = scene->mNumMaterials;
    outInfo.TextureCount = CountMaterialTextures(scene);

    ProcessNode(scene->mRootNode, scene, outMeshes, outInfo);
    outInfo.Loaded = true;

    if (gpfile)
    {
        fprintf(
            gpfile,
            "ModelLoader: Loaded model. Meshes=%llu Vertices=%llu Triangles=%llu Indices=%llu Materials=%llu Textures=%llu ImportTime=%.2fms\n",
            (unsigned long long)outInfo.MeshCount,
            (unsigned long long)outInfo.VertexCount,
            (unsigned long long)outInfo.TriangleCount,
            (unsigned long long)outInfo.IndexCount,
            (unsigned long long)outInfo.MaterialCount,
            (unsigned long long)outInfo.TextureCount,
            outInfo.ImportTimeMs);
    }

    return true;
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes, ModelInfo& outInfo)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        outMeshes.emplace_back(ProcessMesh(mesh, scene, outInfo));
        outInfo.MeshCount++;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, outMeshes, outInfo);
    }
}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelInfo& outInfo)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool hasNormals = mesh->HasNormals();
    bool hasTexCoords = mesh->HasTextureCoords(0);
    bool hasVertexColors = mesh->HasVertexColors(0);

    outInfo.HasNormals = outInfo.HasNormals || hasNormals;
    outInfo.HasTexCoords = outInfo.HasTexCoords || hasTexCoords;
    outInfo.HasVertexColors = outInfo.HasVertexColors || hasVertexColors;

    // Optimization: Reserve memory upfront to avoid reallocations
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        ExpandBounds(outInfo, mesh->mVertices[i]);

        Vertex vertex;
        vertex.Position = vmath::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (hasNormals)
        {
            vertex.Normal = vmath::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else
        {
            vertex.Normal = vmath::vec3(0.0f, 0.0f, 1.0f);
        }

        if (hasTexCoords)
        {
            vertex.TexCoord = vmath::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.TexCoord = vmath::vec2(0.0f, 0.0f);
        }

        if (hasVertexColors)
        {
            vertex.Color = vmath::vec4(
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b,
                mesh->mColors[0][i].a);
        }
        else
        {
            vertex.Color = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    outInfo.VertexCount += mesh->mNumVertices;
    outInfo.TriangleCount += mesh->mNumFaces;
    outInfo.IndexCount += indices.size();

    Mesh m;
    m.Create(vertices, indices);
    return m;
}
