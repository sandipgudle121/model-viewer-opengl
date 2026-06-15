#pragma once

#include <vector>
#include <GL/glew.h>

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Create(
        const std::vector<float>& vertices,
        const std::vector<unsigned int>& indices);

    void Draw();
    void Cleanup();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indexCount;
};