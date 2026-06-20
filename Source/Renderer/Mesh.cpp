#include "Mesh.h"
#include <cstddef>

Mesh::Mesh() : vao(0), vbo(0), ebo(0), diffuseTexture(0), indexCount(0) {}

Mesh::~Mesh() {}

Mesh::Mesh(Mesh&& other) noexcept :
    vao(other.vao),
    vbo(other.vbo),
    ebo(other.ebo),
    diffuseTexture(other.diffuseTexture),
    indexCount(other.indexCount)
{
    other.vao = 0;
    other.vbo = 0;
    other.ebo = 0;
    other.diffuseTexture = 0;
    other.indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        diffuseTexture = other.diffuseTexture;
        indexCount = other.indexCount;

        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.diffuseTexture = 0;
        other.indexCount = 0;
    }

    return *this;
}

void Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, GLuint texture)
{
    Cleanup();

    diffuseTexture = texture;
    indexCount = (GLsizei)indices.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Attribute 1: Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Attribute 2: TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(2);

    // Attribute 3: Color
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Draw()
{
    if (vao != 0)
    {
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (currentProgram != 0)
        {
            GLint hasTextureUniform = glGetUniformLocation((GLuint)currentProgram, "uHasAlbedoTexture");
            if (hasTextureUniform >= 0)
            {
                glUniform1i(hasTextureUniform, diffuseTexture != 0 ? 1 : 0);
            }
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Mesh::Cleanup()
{
    if (ebo)
    {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (diffuseTexture)
    {
        glDeleteTextures(1, &diffuseTexture);
        diffuseTexture = 0;
    }
    indexCount = 0;
}
