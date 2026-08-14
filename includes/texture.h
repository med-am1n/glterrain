#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include <stb_image.h>

#include <iostream>

#include "shader.h"

class Texture
{
public:
    GLuint ID = 0;
    const char *type = nullptr;
    GLuint unit = 0;

    Texture(
        const char *image,
        const char *texType,
        GLuint slot)
    {
        type = texType;
        unit = slot;

        // Flip image vertically
        stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;

        unsigned char *bytes = stbi_load(image, &width, &height, &nrChannels, 0);
        if (!bytes)
        {
            std::cerr << "Failed to load texture: " << image << std::endl;
            return;
        }
        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        GLenum internalFormat = format;

        // Upload image to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(bytes);

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Prevent copying
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    // Move constructor
    Texture(Texture &&other) noexcept
    {
        ID = other.ID;
        type = other.type;
        unit = other.unit;

        other.ID = 0;
    }

    // Move assignment
    Texture &operator=(Texture &&other) noexcept
    {
        if (this != &other)
        {
            Delete();

            ID = other.ID;
            type = other.type;
            unit = other.unit;

            other.ID = 0;
        }

        return *this;
    }

    void texUnit(Shader &shader, const char *uniform)
    {
        shader.use();
        GLint texUni = glGetUniformLocation(shader.ID, uniform);
        if (texUni == -1)
        {
            std::cerr << "Warning: texture uniform '" << uniform << "' was not found." << std::endl;
            return;
        }
        glUniform1i(texUni, unit);
    }

    void Bind()
    {
        if (ID == 0)
            return;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Unbind()
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Delete()
    {
        if (ID != 0)
        {
            glDeleteTextures(1, &ID);
            ID = 0;
        }
    }
    ~Texture()
    {
        Delete();
    }
};

#endif