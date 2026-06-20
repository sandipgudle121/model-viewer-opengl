#include "Texture.h"
#include "../Core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Core/stb_image.h"

namespace Texture
{
    GLuint LoadAlbedoTexture(const std::string& path)
    {
        stbi_set_flip_vertically_on_load(1);

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!pixels)
        {
            if (gpfile) fprintf(gpfile, "Texture: Failed to load albedo texture: %s\n", path.c_str());
            return 0;
        }

        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels);

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(pixels);

        if (gpfile)
        {
            fprintf(gpfile, "Texture: Loaded albedo texture: %s (%dx%d, source channels=%d)\n", path.c_str(), width, height, channels);
        }

        return texture;
    }
}
