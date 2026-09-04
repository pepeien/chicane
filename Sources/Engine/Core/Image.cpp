#include "Chicane/Core/Image.hpp"

#include <fstream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <unordered_map>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#if IS_WINDOWS
    #define STBI_WINDOWS_UTF8
#endif
#include <stb_image.h>

#include "Chicane/Core/FileSystem/Item.hpp"
#include "Chicane/Core/Log.hpp"

namespace
{
    void appendU32(std::vector<unsigned char>& outValue, std::uint32_t inValue)
    {
        outValue.push_back(static_cast<unsigned char>((inValue >> 24) & 0xFF));
        outValue.push_back(static_cast<unsigned char>((inValue >> 16) & 0xFF));
        outValue.push_back(static_cast<unsigned char>((inValue >> 8) & 0xFF));
        outValue.push_back(static_cast<unsigned char>(inValue & 0xFF));
    }

    std::uint32_t crc32(const unsigned char* inData, std::size_t inSize)
    {
        std::uint32_t crc = 0xFFFFFFFFu;

        for (std::size_t i = 0; i < inSize; i++)
        {
            crc ^= inData[i];

            for (int bit = 0; bit < 8; bit++)
            {
                const std::uint32_t mask = static_cast<std::uint32_t>(-(static_cast<int>(crc & 1u)));
                crc                      = (crc >> 1) ^ (0xEDB88320u & mask);
            }
        }

        return crc ^ 0xFFFFFFFFu;
    }

    std::uint32_t adler32(const unsigned char* inData, std::size_t inSize)
    {
        std::uint32_t a = 1;
        std::uint32_t b = 0;

        for (std::size_t i = 0; i < inSize; i++)
        {
            a += inData[i];
            if (a >= 65521)
            {
                a -= 65521;
            }

            b += a;
            if (b >= 65521)
            {
                b -= 65521;
            }
        }

        return (b << 16) | a;
    }

    void appendChunk(
        std::vector<unsigned char>& outValue,
        const char*                 inType,
        const unsigned char*        inData,
        std::size_t                 inSize
    )
    {
        appendU32(outValue, static_cast<std::uint32_t>(inSize));

        const std::size_t crcStart = outValue.size();
        outValue.insert(outValue.end(), inType, inType + 4);
        if (inData != nullptr && inSize > 0)
        {
            outValue.insert(outValue.end(), inData, inData + inSize);
        }

        appendU32(outValue, crc32(outValue.data() + crcStart, 4 + inSize));
    }
}

namespace Chicane
{
    static constexpr int GIF_MIN_DELAY_MS     = 20;
    static constexpr int GIF_DEFAULT_DELAY_MS = 100;

    static const std::unordered_map<ImageVendor, String> EXTENSIONS = {
        {ImageVendor::Undefined, "N/A" },
        {ImageVendor::Jpg,       "JPG" },
        {ImageVendor::Jpg,       "JPEG"},
        {ImageVendor::Png,       "PNG" },
        {ImageVendor::Gif,       "GIF" },
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
        const auto& found = EXTENSIONS.find(inValue);

        if (found == EXTENSIONS.end())
        {
            return EXTENSIONS.at(ImageVendor::Undefined);
        }

        return found->second;
    }

    Image::Image(const FileSystem::Path& inLocation)
        : Image()
    {
        m_vendor = parseVendor(inLocation.extension().toString());
        m_format = STBI_rgb_alpha;

        if (m_vendor == ImageVendor::Gif)
        {
            decodeGif(FileSystem::readUnsigned(inLocation));

            return;
        }

        const String path = inLocation.toString();
        m_pixels          = stbi_load(path.toChar(), &m_width, &m_height, &m_channel, m_format);

        if (!m_pixels)
        {
            throw std::runtime_error(
                "Failed to open [" + inLocation.toString() + "] image (" + String(stbi_failure_reason()) + ")"
            );
        }

        if (m_format > 0)
        {
            m_channel = m_format;
        }

        m_frameCount = 1;
        m_delays     = {0};
    }

    Image::Image(const Raw& inData, ImageVendor inVendor)
        : Image()
    {
        m_vendor = inVendor;
        m_format = STBI_rgb_alpha;

        if (inVendor == ImageVendor::Gif)
        {
            decodeGif(inData);

            return;
        }

        decode(inData);
    }

    Image::Image(const Pixel* inPixels, int inWidth, int inHeight, int inChannel, int inFormat)
        : Image()
    {
        if (!inPixels || inWidth <= 0 || inHeight <= 0 || inChannel <= 0)
        {
            throw std::runtime_error("Failed to copy image frame");
        }

        m_width      = inWidth;
        m_height     = inHeight;
        m_channel    = inChannel;
        m_format     = inFormat;
        m_frameCount = 1;
        m_delays     = {0};

        const std::size_t bytes = static_cast<std::size_t>(inWidth) * static_cast<std::size_t>(inHeight) *
                                  static_cast<std::size_t>(inChannel);
        m_pixels = static_cast<Pixels>(std::malloc(bytes));

        if (!m_pixels)
        {
            throw std::runtime_error("Failed to allocate image frame");
        }

        std::memcpy(m_pixels, inPixels, bytes);
    }

    Image::Image()
        : ImageInfo(),
          m_vendor(ImageVendor::Undefined),
          m_pixels(nullptr),
          m_frameCount(0),
          m_delays({})
    {}

    Image::~Image()
    {
        if (m_pixels == nullptr)
        {
            return;
        }

        stbi_image_free(m_pixels);

        m_pixels = nullptr;
    }

    void Image::decode(const Raw& inData)
    {
        m_pixels = stbi_load_from_memory(
            inData.data(),
            static_cast<int>(inData.size()),
            &m_width,
            &m_height,
            &m_channel,
            m_format
        );

        if (!m_pixels)
        {
            throw std::runtime_error("Failed to parse image data (" + String(stbi_failure_reason()) + ")");
        }

        if (m_format > 0)
        {
            m_channel = m_format;
        }

        m_frameCount = 1;
        m_delays     = {0};
    }

    void Image::decodeGif(const Raw& inData)
    {
        int* delays = nullptr;
        int  x      = 0;
        int  y      = 0;
        int  z      = 0;
        int  comp   = 0;

        m_pixels = stbi_load_gif_from_memory(
            inData.data(),
            static_cast<int>(inData.size()),
            &delays,
            &x,
            &y,
            &z,
            &comp,
            m_format
        );

        if (!m_pixels)
        {
            if (delays)
            {
                STBI_FREE(delays);
            }

            throw std::runtime_error("Failed to parse GIF data (" + String(stbi_failure_reason()) + ")");
        }

        m_width      = x;
        m_height     = y;
        m_channel    = m_format;
        m_frameCount = std::max(1, z);
        m_delays.assign(static_cast<std::size_t>(m_frameCount), GIF_DEFAULT_DELAY_MS);

        if (delays)
        {
            for (int i = 0; i < m_frameCount; i++)
            {
                m_delays[static_cast<std::size_t>(i)] = delays[i] < GIF_MIN_DELAY_MS ? GIF_DEFAULT_DELAY_MS : delays[i];
            }

            STBI_FREE(delays);
        }
    }

    int Image::getFrameStride() const
    {
        return m_width * m_height * m_channel;
    }

    ImageVendor Image::getVendor() const
    {
        return m_vendor;
    }

    int Image::getFrameCount() const
    {
        if (!m_pixels)
        {
            return 0;
        }

        return m_frameCount > 0 ? m_frameCount : 1;
    }

    int Image::getDelay(int inFrame) const
    {
        if (m_delays.empty())
        {
            return 0;
        }

        const int index = std::clamp(inFrame, 0, static_cast<int>(m_delays.size()) - 1);
        const int delay = m_delays[static_cast<std::size_t>(index)];

        return delay < GIF_MIN_DELAY_MS ? GIF_DEFAULT_DELAY_MS : delay;
    }

    const Image::Pixels Image::getPixels() const
    {
        return getPixels(0);
    }

    const Image::Pixels Image::getPixels(int inFrame) const
    {
        if (!m_pixels)
        {
            return nullptr;
        }

        const int count = getFrameCount();
        if (count <= 1)
        {
            return m_pixels;
        }

        const int index = std::clamp(inFrame, 0, count - 1);

        return m_pixels + index * getFrameStride();
    }

    void Image::blit(Pixels outPixels, int outWidth, int outHeight, int inFrame) const
    {
        if (!outPixels || outWidth <= 0 || outHeight <= 0)
        {
            return;
        }

        const Pixels src = getPixels(inFrame);
        if (!src || m_width <= 0 || m_height <= 0)
        {
            std::memset(outPixels, 0, static_cast<std::size_t>(outWidth) * static_cast<std::size_t>(outHeight) * 4);

            return;
        }

        const int srcChannel = m_channel > 0 ? m_channel : 4;

        for (int y = 0; y < outHeight; y++)
        {
            const int srcY = m_height - 1 - ((y * m_height) / outHeight);

            for (int x = 0; x < outWidth; x++)
            {
                const int            srcX = (x * m_width) / outWidth;
                const unsigned char* s    = src + (srcY * m_width + srcX) * srcChannel;
                unsigned char*       d    = outPixels + (y * outWidth + x) * 4;

                d[0] = s[0];
                d[1] = srcChannel > 1 ? s[1] : s[0];
                d[2] = srcChannel > 2 ? s[2] : s[0];
                d[3] = srcChannel > 3 ? s[3] : 255;
            }
        }
    }

    Image::Raw Image::encode() const
    {
        const Pixels pixels = getPixels();
        if (pixels == nullptr || m_width <= 0 || m_height <= 0 || m_channel <= 0)
        {
            return {};
        }

        const int         channels = m_channel;
        const std::size_t rowBytes = static_cast<std::size_t>(m_width) * static_cast<std::size_t>(channels);
        std::vector<unsigned char> raw(static_cast<std::size_t>(m_height) * (rowBytes + 1));

        for (int y = 0; y < m_height; y++)
        {
            const std::size_t offset = static_cast<std::size_t>(y) * (rowBytes + 1);
            raw[offset]              = 0;
            std::memcpy(raw.data() + offset + 1, pixels + static_cast<std::size_t>(y) * rowBytes, rowBytes);
        }

        std::vector<unsigned char> zlib;
        zlib.push_back(0x78);
        zlib.push_back(0x01);

        std::size_t pos = 0;
        while (pos < raw.size())
        {
            const std::size_t block = std::min<std::size_t>(65535, raw.size() - pos);
            zlib.push_back(pos + block >= raw.size() ? 0x01 : 0x00);
            zlib.push_back(static_cast<unsigned char>(block & 0xFF));
            zlib.push_back(static_cast<unsigned char>((block >> 8) & 0xFF));

            const std::uint16_t nlen = static_cast<std::uint16_t>(~static_cast<std::uint16_t>(block));
            zlib.push_back(static_cast<unsigned char>(nlen & 0xFF));
            zlib.push_back(static_cast<unsigned char>((nlen >> 8) & 0xFF));
            zlib.insert(zlib.end(), raw.begin() + static_cast<std::ptrdiff_t>(pos), raw.begin() + static_cast<std::ptrdiff_t>(pos + block));
            pos += block;
        }

        appendU32(zlib, adler32(raw.data(), raw.size()));

        Raw png;
        const unsigned char signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
        png.insert(png.end(), signature, signature + 8);

        unsigned char ihdr[13] = {};
        ihdr[0]                = static_cast<unsigned char>((m_width >> 24) & 0xFF);
        ihdr[1]                = static_cast<unsigned char>((m_width >> 16) & 0xFF);
        ihdr[2]                = static_cast<unsigned char>((m_width >> 8) & 0xFF);
        ihdr[3]                = static_cast<unsigned char>(m_width & 0xFF);
        ihdr[4]                = static_cast<unsigned char>((m_height >> 24) & 0xFF);
        ihdr[5]                = static_cast<unsigned char>((m_height >> 16) & 0xFF);
        ihdr[6]                = static_cast<unsigned char>((m_height >> 8) & 0xFF);
        ihdr[7]                = static_cast<unsigned char>(m_height & 0xFF);
        ihdr[8]                = 8;
        ihdr[9]                = channels == 4 ? 6 : (channels == 3 ? 2 : (channels == 2 ? 4 : 0));
        appendChunk(png, "IHDR", ihdr, sizeof(ihdr));
        appendChunk(png, "IDAT", zlib.data(), zlib.size());
        appendChunk(png, "IEND", nullptr, 0);

        return png;
    }

    std::uint32_t Image::getMemorySize() const
    {
        return sizeof(float) * getSize();
    }

    void Image::flipHorizontally()
    {
        const int rowSize    = m_width * m_channel;
        const int frameCount = std::max(1, getFrameCount());
        const int stride     = getFrameStride();

        for (int frame = 0; frame < frameCount; frame++)
        {
            unsigned char* pixels = m_pixels + frame * stride;

            for (int y = 0; y < m_height; y++)
            {
                unsigned char* row = pixels + y * rowSize;

                for (int x = 0; x < m_width / 2; x++)
                {
                    for (int c = 0; c < m_channel; c++)
                    {
                        std::swap(row[x * m_channel + c], row[(m_width - 1 - x) * m_channel + c]);
                    }
                }
            }
        }
    }

    void Image::flipVertically()
    {
        const int rowSize    = m_width * m_channel;
        const int frameCount = std::max(1, getFrameCount());
        const int stride     = getFrameStride();

        for (int frame = 0; frame < frameCount; frame++)
        {
            unsigned char* pixels = m_pixels + frame * stride;

            for (int y = 0; y < m_height / 2; y++)
            {
                unsigned char* topRow    = pixels + y * rowSize;
                unsigned char* bottomRow = pixels + (m_height - 1 - y) * rowSize;

                for (int x = 0; x < rowSize; x++)
                {
                    std::swap(topRow[x], bottomRow[x]);
                }
            }
        }
    }

    void Image::rotateBy(float inAngle)
    {
        const float angle = inAngle * (M_PI / 180.0f);
        const float cosA  = std::cos(angle);
        const float sinA  = std::sin(angle);

        const int pixelCount = getFrameStride();
        const int frameCount = std::max(1, getFrameCount());

        const float cx = (m_width - 1) * 0.5f;
        const float cy = (m_height - 1) * 0.5f;

        for (int frame = 0; frame < frameCount; frame++)
        {
            unsigned char*             pixels = m_pixels + frame * pixelCount;
            std::vector<unsigned char> src(pixels, pixels + pixelCount);

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

                    unsigned char* dst = pixels + m_channel * (y * m_width + x);

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
}
