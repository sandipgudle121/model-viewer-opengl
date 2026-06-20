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
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Create(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        GLuint diffuseTexture = 0);

    void Draw();
    void Cleanup();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint diffuseTexture;
    GLsizei indexCount;
};
