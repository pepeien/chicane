#include "Chicane/Core/Image.hpp"

#include <unordered_map>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Chicane
{
    static const std::unordered_map<ImageVendor, String> EXTENSIONS = {
        {ImageVendor::Undefined, "N/A" },
        {ImageVendor::Jpg,       "JPG" },
        {ImageVendor::Jpg,       "JPEG"},
        {ImageVendor::Png,       "PNG" },
    };

    ImageVendor Image::parseVendor(const String& inValue)
    {
        if (inValue.isEmpty())
        {
            return ImageVendor::Undefined;
        }

        const String& value = inValue.trim().toUpper();

        for (const auto& [type, extension] : EXTENSIONS)
        {
            if (!value.contains(extension))
            {
                continue;
            }

            return type;
        }

        return ImageVendor::Undefined;
    }

    const String& Image::getVendorExtension(ImageVendor inValue)
    {
        if (EXTENSIONS.find(inValue) == EXTENSIONS.end())
        {
            return EXTENSIONS.at(ImageVendor::Undefined);
        }

        return EXTENSIONS.at(inValue);
    }

    Image::Image(const FileSystem::Path& inLocation, bool bInShouldFlip)
        : Image()
    {
        stbi_set_flip_vertically_on_load_thread(bInShouldFlip);

        m_vendor = parseVendor(inLocation.extension());

        m_format = STBI_rgb_alpha;

        m_pixels = stbi_load(inLocation.string().c_str(), &m_width, &m_height, &m_channel, m_format);

        if (!m_pixels)
        {
            throw std::runtime_error(stbi_failure_reason());
        }
    }

    Image::Image(const Raw& inData, ImageVendor inVendor, bool bInShouldFlip)
        : Image()
    {
        stbi_set_flip_vertically_on_load_thread(bInShouldFlip);

        m_vendor = inVendor;

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
        : m_vendor(ImageVendor::Undefined),
          m_width(0),
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

    ImageVendor Image::getVendor() const
    {
        return m_vendor;
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

    void Image::rotate(float inAngle)
    {
        float angle = inAngle * (M_PI / 180.0f);

        std::vector<unsigned char> src(m_pixels, m_pixels + m_width * m_height * m_channel);

        int cx = m_width / 2;
        int cy = m_height / 2;

        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                float dx = x - cx;
                float dy = y - cy;

                float fx = std::cos(angle) * dx - std::sin(angle) * dy + cx;
                float fy = std::sin(angle) * dx + std::cos(angle) * dy + cy;

                int x2 = static_cast<int>(std::round(fx));
                int y2 = static_cast<int>(std::round(fy));

                unsigned char* dst = m_pixels + m_channel * (y * m_width + x);

                if (x2 < 0 || x2 >= m_width || y2 < 0 || y2 >= m_height)
                {
                    std::fill(dst, dst + m_channel, 0);
                }
                else
                {
                    unsigned char* srcp = &src[m_channel * (y2 * m_width + x2)];
                    std::copy(srcp, srcp + m_channel, dst);
                }
            }
        }
    }
}