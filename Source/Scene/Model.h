#pragma once

#include <string>
#include <vector>
#include "../Renderer/Mesh.h"

class Model
{
public:
    void Draw();
    void Cleanup();
    bool Load(const std::string& path);
    void LoadCube(); // Hardcoded cube mesh generator

private:
    std::vector<Mesh> Meshes;
};