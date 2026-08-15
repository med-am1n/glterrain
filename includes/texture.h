#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include <stb_image.h>

#include <iostream>

#include "shader.h"

enum TextureType
{
    Diffuse,
    Specular,
    Normal,
    Height
};

class Texture
{
public:
    GLuint ID = 0;

    TextureType type;
    std::string path;

    Texture(
        const char *image,
        TextureType texType)
        : type(texType),
          path(image)
    {

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
        // glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
        {
            std::cerr << "Unsupported texture format: " << nrChannels << " channels" << std::endl;
            stbi_image_free(bytes);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDeleteTextures(1, &ID);
            ID = 0;

            return;
        }

        // Upload image to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
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
        path = other.path;

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
            path = other.path;

            other.ID = 0;
        }

        return *this;
    }

    void texUnit(
        Shader &shader,
        const char *uniform,
        GLuint unit)
    {
        shader.use();

        GLint texUni =
            glGetUniformLocation(shader.ID, uniform);

        if (texUni == -1)
        {
            std::cerr << "Warning: texture uniform '" << uniform << "' was not found." << std::endl;
            return;
        }

        glUniform1i(texUni, unit);
    }

    void Bind(GLuint unit)
    {
        if (ID == 0)
            return;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Unbind(GLuint unit)
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