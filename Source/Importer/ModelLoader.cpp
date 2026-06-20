#include "ModelLoader.h"
#include "../Core/Logger.h"
#include "../Renderer/Texture.h"
#include "../Scene/Model.h"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <cmath>

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

    aiVector3D TransformPosition(const aiMatrix4x4& transform, const aiVector3D& position)
    {
        return aiVector3D(
            transform.a1 * position.x + transform.a2 * position.y + transform.a3 * position.z + transform.a4,
            transform.b1 * position.x + transform.b2 * position.y + transform.b3 * position.z + transform.b4,
            transform.c1 * position.x + transform.c2 * position.y + transform.c3 * position.z + transform.c4);
    }

    aiVector3D TransformNormal(const aiMatrix4x4& transform, const aiVector3D& normal)
    {
        aiVector3D result(
            transform.a1 * normal.x + transform.a2 * normal.y + transform.a3 * normal.z,
            transform.b1 * normal.x + transform.b2 * normal.y + transform.b3 * normal.z,
            transform.c1 * normal.x + transform.c2 * normal.y + transform.c3 * normal.z);

        float length = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z);
        if (length > 0.0f)
        {
            result.x /= length;
            result.y /= length;
            result.z /= length;
        }

        return result;
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

    void LogUVDiagnostics(const aiMesh* mesh, bool hasTexCoords)
    {
        if (!gpfile)
        {
            return;
        }

        const char* meshName = mesh->mName.C_Str();
        if (!meshName || meshName[0] == '\0')
        {
            meshName = "(unnamed)";
        }

        fprintf(gpfile, "ModelLoader: Mesh '%s' UV0: %s\n", meshName, hasTexCoords ? "YES" : "NO");
        if (!hasTexCoords || mesh->mNumVertices == 0)
        {
            return;
        }

        float minU = mesh->mTextureCoords[0][0].x;
        float minV = mesh->mTextureCoords[0][0].y;
        float maxU = minU;
        float maxV = minV;

        for (unsigned int i = 1; i < mesh->mNumVertices; i++)
        {
            float u = mesh->mTextureCoords[0][i].x;
            float v = mesh->mTextureCoords[0][i].y;

            if (u < minU) minU = u;
            if (v < minV) minV = v;
            if (u > maxU) maxU = u;
            if (v > maxV) maxV = v;
        }

        fprintf(gpfile, "ModelLoader: Mesh '%s' UV0 Bounds Min(%.6f, %.6f) Max(%.6f, %.6f)\n", meshName, minU, minV, maxU, maxV);

        unsigned int sampleCount = mesh->mNumVertices < 5 ? mesh->mNumVertices : 5;
        for (unsigned int i = 0; i < sampleCount; i++)
        {
            fprintf(
                gpfile,
                "ModelLoader: Mesh '%s' UV0[%u] = (%.6f, %.6f)\n",
                meshName,
                i,
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y);
        }
    }

    void FinalizeBounds(ModelInfo& info)
    {
        if (!info.HasBounds)
        {
            return;
        }

        info.BoundsCenter = vmath::vec3(
            (info.BoundsMin[0] + info.BoundsMax[0]) * 0.5f,
            (info.BoundsMin[1] + info.BoundsMax[1]) * 0.5f,
            (info.BoundsMin[2] + info.BoundsMax[2]) * 0.5f);

        vmath::vec3 extent = vmath::vec3(
            info.BoundsMax[0] - info.BoundsMin[0],
            info.BoundsMax[1] - info.BoundsMin[1],
            info.BoundsMax[2] - info.BoundsMin[2]);

        info.BoundsRadius = 0.5f * sqrtf(extent[0] * extent[0] + extent[1] * extent[1] + extent[2] * extent[2]);
        if (info.BoundsRadius <= 0.0f)
        {
            info.BoundsRadius = 1.0f;
        }

        float maxExtent = extent[0];
        if (extent[1] > maxExtent) maxExtent = extent[1];
        if (extent[2] > maxExtent) maxExtent = extent[2];
        if (maxExtent <= 0.0f)
        {
            maxExtent = 1.0f;
        }
        info.NormalizedScale = 2.0f / maxExtent;
    }

    std::string ResolveTexturePath(const std::string& modelDirectory, const aiString& texturePath)
    {
        namespace fs = std::filesystem;

        fs::path rawPath(texturePath.C_Str());
        if (rawPath.empty())
        {
            return "";
        }

        if (rawPath.is_absolute() && fs::exists(rawPath))
        {
            return rawPath.string();
        }

        fs::path directory(modelDirectory);
        fs::path directPath = directory / rawPath;
        if (fs::exists(directPath))
        {
            return directPath.string();
        }

        fs::path filename = rawPath.filename();
        fs::path siblingTexturePath = directory / "textures" / filename;
        if (fs::exists(siblingTexturePath))
        {
            return siblingTexturePath.string();
        }

        fs::path parentTexturePath = directory.parent_path() / "textures" / filename;
        if (fs::exists(parentTexturePath))
        {
            return parentTexturePath.string();
        }

        return directPath.string();
    }

    GLuint LoadDiffuseTextureForMesh(aiMesh* mesh, const aiScene* scene, const std::string& modelDirectory)
    {
        if (mesh->mMaterialIndex >= scene->mNumMaterials)
        {
            return 0;
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (!material || material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
        {
            return 0;
        }

        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
        {
            return 0;
        }

        std::string resolvedPath = ResolveTexturePath(modelDirectory, texturePath);
        if (gpfile)
        {
            fprintf(
                gpfile,
                "ModelLoader: Mesh '%s' diffuse texture: %s -> %s\n",
                mesh->mName.length > 0 ? mesh->mName.C_Str() : "(unnamed)",
                texturePath.C_Str(),
                resolvedPath.c_str());
        }

        return Texture::LoadAlbedoTexture(resolvedPath);
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

    std::filesystem::path modelPath(path);
    std::string modelDirectory = modelPath.parent_path().string();

    ProcessNode(scene->mRootNode, scene, aiMatrix4x4(), modelDirectory, outMeshes, outInfo);
    FinalizeBounds(outInfo);
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

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& parentTransform, const std::string& modelDirectory, std::vector<Mesh>& outMeshes, ModelInfo& outInfo)
{
    aiMatrix4x4 nodeTransform = parentTransform * node->mTransformation;
    outInfo.HasNodeTransforms = true;

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        outMeshes.emplace_back(ProcessMesh(mesh, scene, nodeTransform, modelDirectory, outInfo));
        outInfo.MeshCount++;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, nodeTransform, modelDirectory, outMeshes, outInfo);
    }
}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& nodeTransform, const std::string& modelDirectory, ModelInfo& outInfo)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool hasNormals = mesh->HasNormals();
    bool hasTexCoords = mesh->HasTextureCoords(0);
    bool hasVertexColors = mesh->HasVertexColors(0);

    outInfo.HasNormals = outInfo.HasNormals || hasNormals;
    outInfo.HasTexCoords = outInfo.HasTexCoords || hasTexCoords;
    outInfo.HasVertexColors = outInfo.HasVertexColors || hasVertexColors;

    LogUVDiagnostics(mesh, hasTexCoords);

    // Optimization: Reserve memory upfront to avoid reallocations
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D transformedPosition = TransformPosition(nodeTransform, mesh->mVertices[i]);
        ExpandBounds(outInfo, transformedPosition);

        Vertex vertex;
        vertex.Position = vmath::vec3(transformedPosition.x, transformedPosition.y, transformedPosition.z);

        if (hasNormals)
        {
            aiVector3D transformedNormal = TransformNormal(nodeTransform, mesh->mNormals[i]);
            vertex.Normal = vmath::vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z);
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

    GLuint diffuseTexture = LoadDiffuseTextureForMesh(mesh, scene, modelDirectory);
    outInfo.HasAlbedoTexture = outInfo.HasAlbedoTexture || (diffuseTexture != 0);

    Mesh m;
    m.Create(vertices, indices, diffuseTexture);
    return m;
}
