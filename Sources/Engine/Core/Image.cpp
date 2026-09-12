#include "Chicane/Core/Image.hpp"

#include <fstream>
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
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

static void appendU32(std::vector<unsigned char>& outValue, std::uint32_t inValue)
{
    outValue.push_back(static_cast<unsigned char>((inValue >> 24) & 0xFF));
    outValue.push_back(static_cast<unsigned char>((inValue >> 16) & 0xFF));
    outValue.push_back(static_cast<unsigned char>((inValue >> 8) & 0xFF));
    outValue.push_back(static_cast<unsigned char>(inValue & 0xFF));
}

static std::uint32_t crc32(const unsigned char* inData, std::size_t inSize)
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

static std::uint32_t adler32(const unsigned char* inData, std::size_t inSize)
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

static std::uint32_t bitReverse(std::uint32_t inValue, int inBits)
{
    std::uint32_t result = 0;
    for (int i = 0; i < inBits; i++)
    {
        result = (result << 1) | (inValue & 1u);
        inValue >>= 1;
    }

    return result;
}

static void writeBits(
    std::vector<unsigned char>& outValue, std::uint32_t& outBuffer, int& outCount, std::uint32_t inBits, int inSize
)
{
    outBuffer |= inBits << outCount;
    outCount += inSize;
    while (outCount >= 8)
    {
        outValue.push_back(static_cast<unsigned char>(outBuffer));
        outBuffer >>= 8;
        outCount -= 8;
    }
}

static void writeFixedHuffman(
    std::vector<unsigned char>& outValue, std::uint32_t& outBuffer, int& outCount, int inSymbol
)
{
    if (inSymbol <= 143)
    {
        writeBits(outValue, outBuffer, outCount, bitReverse(0x30u + static_cast<std::uint32_t>(inSymbol), 8), 8);
    }
    else if (inSymbol <= 255)
    {
        writeBits(
            outValue,
            outBuffer,
            outCount,
            bitReverse(0x190u + static_cast<std::uint32_t>(inSymbol - 144), 9),
            9
        );
    }
    else if (inSymbol <= 279)
    {
        writeBits(
            outValue,
            outBuffer,
            outCount,
            bitReverse(static_cast<std::uint32_t>(inSymbol - 256), 7),
            7
        );
    }
    else
    {
        writeBits(
            outValue,
            outBuffer,
            outCount,
            bitReverse(0xC0u + static_cast<std::uint32_t>(inSymbol - 280), 8),
            8
        );
    }
}

static void writeLengthDistance(
    std::vector<unsigned char>& outValue,
    std::uint32_t&              outBuffer,
    int&                        outCount,
    int                         inLength,
    int                         inDistance
)
{
    static const int lengthBase[29] = {3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
                                       31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    static const int lengthExtra[29] =
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    static const int distanceBase[30] = {1,   2,   3,   4,   5,   7,    9,    13,   17,   25,
                                         33,  49,  65,  97,  129, 193,  257,  385,  513,  769,
                                         1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
    static const int distanceExtra[30] =
        {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

    int lengthCode = 0;
    for (int i = 0; i < 28; i++)
    {
        if (inLength >= lengthBase[i + 1])
        {
            lengthCode = i + 1;
        }
    }
    writeFixedHuffman(outValue, outBuffer, outCount, 257 + lengthCode);
    if (lengthExtra[lengthCode] > 0)
    {
        writeBits(
            outValue,
            outBuffer,
            outCount,
            static_cast<std::uint32_t>(inLength - lengthBase[lengthCode]),
            lengthExtra[lengthCode]
        );
    }

    int distanceCode = 0;
    for (int i = 0; i < 29; i++)
    {
        if (inDistance >= distanceBase[i + 1])
        {
            distanceCode = i + 1;
        }
    }
    writeBits(outValue, outBuffer, outCount, bitReverse(static_cast<std::uint32_t>(distanceCode), 5), 5);
    if (distanceExtra[distanceCode] > 0)
    {
        writeBits(
            outValue,
            outBuffer,
            outCount,
            static_cast<std::uint32_t>(inDistance - distanceBase[distanceCode]),
            distanceExtra[distanceCode]
        );
    }
}

static std::vector<unsigned char> deflateZlib(const unsigned char* inData, std::size_t inSize)
{
    std::vector<unsigned char> zlib;
    zlib.push_back(0x78);
    zlib.push_back(0x9C);

    std::uint32_t bitBuffer = 0;
    int           bitCount  = 0;
    writeBits(zlib, bitBuffer, bitCount, 1, 1);
    writeBits(zlib, bitBuffer, bitCount, 1, 2);

    constexpr int kWindow = 32768;
    constexpr int kHash   = 32768;
    std::vector<int> head(kHash, -1);
    std::vector<int> prev(kWindow, -1);

    auto hashOf = [](const unsigned char* inBytes) -> int
    {
        return static_cast<int>(
            ((static_cast<std::uint32_t>(inBytes[0]) << 16) ^ (static_cast<std::uint32_t>(inBytes[1]) << 8) ^
             static_cast<std::uint32_t>(inBytes[2])) &
            32767u
        );
    };

    std::size_t index = 0;
    while (index < inSize)
    {
        int bestLength   = 0;
        int bestDistance = 0;
        if (index + 2 < inSize)
        {
            const int hash     = hashOf(inData + index);
            int       candidate = head[hash];
            int       steps     = 0;
            while (candidate >= 0 && steps < 64)
            {
                const int distance = static_cast<int>(index) - candidate;
                if (distance <= 0 || distance > kWindow)
                {
                    break;
                }

                int match = 0;
                while (index + static_cast<std::size_t>(match) < inSize && match < 258 &&
                       inData[index + static_cast<std::size_t>(match)] ==
                           inData[static_cast<std::size_t>(candidate) + static_cast<std::size_t>(match)])
                {
                    match++;
                }

                if (match > bestLength && match >= 3)
                {
                    bestLength   = match;
                    bestDistance = distance;
                    if (bestLength >= 258)
                    {
                        break;
                    }
                }

                candidate = prev[candidate & (kWindow - 1)];
                steps++;
            }

            prev[static_cast<std::size_t>(static_cast<int>(index) & (kWindow - 1))] = head[hash];
            head[hash]                                                             = static_cast<int>(index);
        }

        if (bestLength >= 3)
        {
            writeLengthDistance(zlib, bitBuffer, bitCount, bestLength, bestDistance);
            for (int offset = 1; offset < bestLength && index + static_cast<std::size_t>(offset) + 2 < inSize; offset++)
            {
                const int hash = hashOf(inData + index + static_cast<std::size_t>(offset));
                prev[static_cast<std::size_t>((static_cast<int>(index) + offset) & (kWindow - 1))] = head[hash];
                head[hash] = static_cast<int>(index) + offset;
            }
            index += static_cast<std::size_t>(bestLength);

            continue;
        }

        writeFixedHuffman(zlib, bitBuffer, bitCount, inData[index]);
        index++;
    }

    writeFixedHuffman(zlib, bitBuffer, bitCount, 256);
    if (bitCount > 0)
    {
        zlib.push_back(static_cast<unsigned char>(bitBuffer));
    }

    appendU32(zlib, adler32(inData, inSize));

    return zlib;
}

static void appendChunk(
    std::vector<unsigned char>& outValue, const char* inType, const unsigned char* inData, std::size_t inSize
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
            const int srcY = (y * m_height) / outHeight;

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

        const int                  channels = m_channel;
        const std::size_t          rowBytes = static_cast<std::size_t>(m_width) * static_cast<std::size_t>(channels);
        std::vector<unsigned char> raw(static_cast<std::size_t>(m_height) * (rowBytes + 1));

        for (int y = 0; y < m_height; y++)
        {
            const std::size_t offset = static_cast<std::size_t>(y) * (rowBytes + 1);
            raw[offset]              = 0;
            std::memcpy(raw.data() + offset + 1, pixels + static_cast<std::size_t>(y) * rowBytes, rowBytes);
        }

        std::vector<unsigned char> zlib = deflateZlib(raw.data(), raw.size());

        Raw                 png;
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
        const float angle = inAngle * DEG_TO_RAD;
        const float cosA  = std::cos(angle);
        const float sinA  = std::sin(angle);

        const int pixelCount = getFrameStride();
        const int frameCount = std::max(1, getFrameCount());

        const Vec2 center((m_width - 1) * 0.5f, (m_height - 1) * 0.5f);

        for (int frame = 0; frame < frameCount; frame++)
        {
            unsigned char*             pixels = m_pixels + frame * pixelCount;
            std::vector<unsigned char> src(pixels, pixels + pixelCount);

            for (int y = 0; y < m_height; y++)
            {
                const float dy = y - center.y;

                for (int x = 0; x < m_width; x++)
                {
                    const Vec2 delta(x - center.x, dy);
                    Vec2       source(
                        cosA * delta.x + sinA * delta.y + center.x,
                        -sinA * delta.x + cosA * delta.y + center.y
                    );

                    source.x = std::clamp(source.x, 0.0f, (float)(m_width - 1));
                    source.y = std::clamp(source.y, 0.0f, (float)(m_height - 1));

                    int x0 = (int)source.x;
                    int y0 = (int)source.y;
                    int x1 = std::min(x0 + 1, m_width - 1);
                    int y1 = std::min(y0 + 1, m_height - 1);

                    const Vec2 t(source.x - x0, source.y - y0);

                    unsigned char* dst = pixels + m_channel * (y * m_width + x);

                    unsigned char* p00 = &src[m_channel * (y0 * m_width + x0)];
                    unsigned char* p10 = &src[m_channel * (y0 * m_width + x1)];
                    unsigned char* p01 = &src[m_channel * (y1 * m_width + x0)];
                    unsigned char* p11 = &src[m_channel * (y1 * m_width + x1)];

                    for (int c = 0; c < m_channel; c++)
                    {
                        float v = (1 - t.x) * (1 - t.y) * p00[c] + t.x * (1 - t.y) * p10[c] + (1 - t.x) * t.y * p01[c] +
                                  t.x * t.y * p11[c];

                        dst[c] = (unsigned char)(v + 0.5f);
                    }
                }
            }
        }
    }
}
