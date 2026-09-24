#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Info.hpp"
#include "Chicane/Core/Image/Mip.hpp"
#include "Chicane/Core/Image/Mip/Chain.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    struct CHICANE_CORE Image : public ImageInfo
    {
    public:
        using Raw         = std::vector<unsigned char>;
        using Pixel       = unsigned char;
        using Pixels      = unsigned char*;
        using FloatPixel  = float;
        using FloatPixels = float*;

        using Instance   = std::shared_ptr<const Image>;
        using Reference  = std::weak_ptr<const Image>;
        using References = std::vector<Reference>;

        static constexpr inline const std::uint32_t MAX_SIZE    = 4096;
        static constexpr inline const std::uint32_t STREAM_TAIL = 128;

    public:
        static ImageVendor parseVendor(const String& inValue);
        static const String& getVendorExtension(ImageVendor inValue);

        static std::uint32_t floorPowerOfTwo(std::uint32_t inValue);
        static std::uint32_t mipCount(std::uint32_t inWidth, std::uint32_t inHeight);
        static std::uint32_t mipDimension(std::uint32_t inSize, std::uint32_t inLevel);
        static std::uint32_t streamTailMinMip(
            std::uint32_t inWidth, std::uint32_t inHeight, std::uint32_t inTail = STREAM_TAIL
        );
        static std::size_t mipChainBytes(std::uint32_t inWidth, std::uint32_t inHeight);
        static ImageMipChain makeMipChain(
            const Image& inSource, std::uint32_t inMaxSize = MAX_SIZE, bool inIsNormal = false
        );
        static ImageMipChain makeMipChain(
            const Pixel*  inPixels,
            int           inWidth,
            int           inHeight,
            int           inChannel,
            std::uint32_t inMaxSize  = MAX_SIZE,
            bool          inIsNormal = false
        );
        static void flipY(Pixels inPixels, int inWidth, int inHeight, int inChannel);

    public:
        Image(const FileSystem::Path& inLocation);
        Image(const Raw& inData, ImageVendor inVendor);
        Image(const Pixel* inPixels, int inWidth, int inHeight, int inChannel, int inFormat);
        Image();

        virtual ~Image();

    public:
        ImageVendor getVendor() const;
        bool isHdr() const;
        int getFrameCount() const;
        int getDelay(int inFrame = 0) const;
        const Pixels getPixels() const;
        const Pixels getPixels(int inFrame) const;
        const FloatPixels getFloatPixels() const;
        void blit(Pixels outPixels, int outWidth, int outHeight, int inFrame = 0) const;
        void blitFloat(FloatPixels outPixels, int outWidth, int outHeight) const;
        Raw encode() const;

        std::uint32_t getMemorySize() const;

        void flipHorizontally();
        void flipVertically();
        void rotateBy(float inAngle);

    protected:
        void decode(const Raw& inData);
        void decodeGif(const Raw& inData);
        bool decodeHdr(const Raw& inData);
        bool decodeHdrFile(const FileSystem::Path& inLocation);
        void rebuildLdrFromHdr();
        int getFrameStride() const;

    protected:
        ImageVendor      m_vendor;
        Pixels           m_pixels;
        FloatPixels      m_floats;
        int              m_frameCount;
        std::vector<int> m_delays;
    };
}