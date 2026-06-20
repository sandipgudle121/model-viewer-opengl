#pragma once

#include <vector>
#include <GL/glew.h>
#include "../vmath.h"

struct Vertex
{
    vmath::vec3 Position;
    vmath::vec3 Normal;
    vmath::vec2 TexCoord;
    vmath::vec4 Color;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Create(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    void Draw();
    void Cleanup();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indexCount;
};
