#include "ui/GLTexture.hpp"
#include "core/Logger.hpp"

#include <GLFW/glfw3.h>

// Define GL_CLAMP_TO_EDGE if not available (Windows OpenGL 1.1 headers)
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace NanoRec
{

    GLTexture::GLTexture()
        : m_textureID(0), m_width(0), m_height(0), m_channels(0), m_format(0), m_internalFormat(0)
    {
    }

    GLTexture::~GLTexture()
    {
        destroy();
    }

    bool GLTexture::create(int width, int height, const uint8_t *data, int channels)
    {
        if (width <= 0 || height <= 0 || channels < 3 || channels > 4)
        {
            Logger::error("Invalid texture parameters");
            return false;
        }

        // Destroy existing texture if any
        if (m_textureID != 0)
        {
            destroy();
        }

        m_width = width;
        m_height = height;
        m_channels = channels;

        // Set format based on channels
        if (channels == 3)
        {
            m_format = GL_RGB;
            m_internalFormat = GL_RGB8;
        }
        else // channels == 4
        {
            m_format = GL_RGBA;
            m_internalFormat = GL_RGBA8;
        }

        // Generate texture
        glGenTextures(1, &m_textureID);
        if (m_textureID == 0)
        {
            Logger::error("Failed to generate OpenGL texture");
            return false;
        }

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);

        // Check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            Logger::error("OpenGL error creating texture: " + std::to_string(error));
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
            return false;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        Logger::info("Created OpenGL texture: " + std::to_string(m_width) + "x" +
                     std::to_string(m_height) + " (" + std::to_string(m_channels) + " channels)");

        return true;
    }

    bool GLTexture::update(const uint8_t *data)
    {
        if (m_textureID == 0)
        {
            Logger::error("Cannot update texture: texture not created");
            return false;
        }

        if (data == nullptr)
        {
            Logger::error("Cannot update texture: data is null");
            return false;
        }

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // Update texture data (more efficient than glTexImage2D for updates)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);

        // Check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            Logger::error("OpenGL error updating texture: " + std::to_string(error));
            glBindTexture(GL_TEXTURE_2D, 0);
            return false;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void GLTexture::destroy()
    {
        if (m_textureID != 0)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
            m_width = 0;
            m_height = 0;
            m_channels = 0;
        }
    }

} // namespace NanoRec
