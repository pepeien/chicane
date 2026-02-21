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
        const float angle = inAngle * (M_PI / 180.0f);
        const float cosA  = std::cos(angle);
        const float sinA  = std::sin(angle);

        const int pixelCount = m_width * m_height * m_channel;

        std::vector<unsigned char> src(m_pixels, m_pixels + pixelCount);

        const float cx = (m_width - 1) * 0.5f;
        const float cy = (m_height - 1) * 0.5f;

        for (int y = 0; y < m_height; y++)
        {
            float dy = y - cy;

            for (int x = 0; x < m_width; x++)
            {
                float dx = x - cx;

                float srcX = cosA * dx + sinA * dy + cx;
                float srcY = -sinA * dx + cosA * dy + cy;

                srcX = std::clamp(srcX, 0.0f, (float)(m_width - 1));
                srcY = std::clamp(srcY, 0.0f, (float)(m_height - 1));

                int x0 = (int)srcX;
                int y0 = (int)srcY;
                int x1 = std::min(x0 + 1, m_width - 1);
                int y1 = std::min(y0 + 1, m_height - 1);

                float tx = srcX - x0;
                float ty = srcY - y0;

                unsigned char* dst = m_pixels + m_channel * (y * m_width + x);

                unsigned char* p00 = &src[m_channel * (y0 * m_width + x0)];
                unsigned char* p10 = &src[m_channel * (y0 * m_width + x1)];
                unsigned char* p01 = &src[m_channel * (y1 * m_width + x0)];
                unsigned char* p11 = &src[m_channel * (y1 * m_width + x1)];

                for (int c = 0; c < m_channel; c++)
                {
                    float v = (1 - tx) * (1 - ty) * p00[c] + tx * (1 - ty) * p10[c] + (1 - tx) * ty * p01[c] +
                              tx * ty * p11[c];

                    dst[c] = (unsigned char)(v + 0.5f);
                }
            }
        }
    }
}