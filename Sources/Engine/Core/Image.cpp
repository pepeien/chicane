#include "Chicane/Core/Image.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Chicane
{
    Image::Image(const FileSystem::Path& inFilepath)
        : Image()
    {
        m_format = STBI_rgb_alpha;

        m_pixels = stbi_load(inFilepath.string().c_str(), &m_width, &m_height, &m_channel, m_format);

        if (!m_pixels)
        {
            throw std::runtime_error(stbi_failure_reason());
        }
    }

    Image::Image(const Raw& inData)
        : Image()
    {
        m_format = STBI_rgb_alpha;

        m_pixels = stbi_load_from_memory(
            &inData.at(0),
            static_cast<int>(inData.size()),
            &m_width,
            &m_height,
            &m_channel,
            m_format
        );

        if (!m_pixels)
        {
            throw std::runtime_error(stbi_failure_reason());
        }
    }

    Image::Image()
        : m_width(0),
          m_height(0),
          m_channel(0),
          m_format(0),
          m_pixels(nullptr)
    {}

    Image::Image(const Image& inImage)
        : m_width(inImage.m_width),
          m_height(inImage.m_height),
          m_channel(inImage.m_channel),
          m_format(inImage.m_format),
          m_pixels(inImage.m_pixels)
    {}

    Image::~Image()
    {
        if (m_pixels != nullptr)
        {
            return;
        }

        stbi_image_free(m_pixels);

        m_pixels = nullptr;
    }

    int Image::getWidth() const
    {
        return m_width;
    }

    int Image::getHeight() const
    {
        return m_height;
    }

    int Image::getChannel() const
    {
        return m_channel;
    }

    int Image::getFormat() const
    {
        return m_format;
    }

    const Image::Pixels Image::getPixels() const
    {
        return m_pixels;
    }

    int Image::getPitch() const
    {
        return m_format * m_width;
    }

    int Image::getSize() const
    {
        return m_width * m_height;
    }
}