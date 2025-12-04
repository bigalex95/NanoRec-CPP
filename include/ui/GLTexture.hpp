#pragma once

#include <cstdint>
#include <vector>

namespace NanoRec
{

    /**
     * @brief RAII wrapper for OpenGL textures
     *
     * Manages OpenGL texture lifecycle with automatic cleanup.
     * Supports RGB/RGBA formats and efficient texture updates.
     */
    class GLTexture
    {
    public:
        GLTexture();
        ~GLTexture();

        // Delete copy constructor and assignment operator
        GLTexture(const GLTexture &) = delete;
        GLTexture &operator=(const GLTexture &) = delete;

        /**
         * @brief Create texture from image data
         * @param width Texture width in pixels
         * @param height Texture height in pixels
         * @param data Pointer to RGB/RGBA pixel data
         * @param channels Number of color channels (3 for RGB, 4 for RGBA)
         * @return true if texture created successfully
         */
        bool create(int width, int height, const uint8_t *data, int channels = 3);

        /**
         * @brief Update existing texture with new data
         * @param data Pointer to new pixel data
         * @return true if update successful
         */
        bool update(const uint8_t *data);

        /**
         * @brief Get OpenGL texture ID
         * @return Texture ID (0 if not created)
         */
        unsigned int getTextureID() const { return m_textureID; }

        /**
         * @brief Get texture width
         */
        int getWidth() const { return m_width; }

        /**
         * @brief Get texture height
         */
        int getHeight() const { return m_height; }

        /**
         * @brief Check if texture is valid
         */
        bool isValid() const { return m_textureID != 0; }

        /**
         * @brief Destroy texture and free resources
         */
        void destroy();

    private:
        unsigned int m_textureID;
        int m_width;
        int m_height;
        int m_channels;
        unsigned int m_format;      // GL_RGB or GL_RGBA
        unsigned int m_internalFormat; // GL_RGB8 or GL_RGBA8
    };

} // namespace NanoRec
