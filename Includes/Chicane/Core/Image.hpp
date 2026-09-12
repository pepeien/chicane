#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Info.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    struct CHICANE_CORE Image : public ImageInfo
    {
    public:
        using Raw    = std::vector<unsigned char>;
        using Pixel  = unsigned char;
        using Pixels = unsigned char*;

        using Instance   = std::shared_ptr<const Image>;
        using Reference  = std::weak_ptr<const Image>;
        using References = std::vector<Reference>;

        static constexpr inline const std::uint32_t MAX_SIZE     = 4096;
        static constexpr inline const std::uint32_t STREAM_TAIL  = 128;

        struct CHICANE_CORE Mip
        {
            int              width   = 0;
            int              height  = 0;
            Raw              encoded = {};
            mutable Instance decoded = {};
        };

        struct CHICANE_CORE MipChain
        {
            std::vector<Mip> levels  = {};
            bool             bNormal = false;

            bool isEmpty() const;
            std::uint32_t getWidth() const;
            std::uint32_t getHeight() const;
            std::uint32_t getCount() const;
            std::uint32_t streamTailMinMip(std::uint32_t inTail = STREAM_TAIL) const;
            std::size_t residentBytes(std::uint32_t inMinMip) const;

            Instance decode(std::uint32_t inLevel) const;
            void ensureDecoded(std::uint32_t inMinLevel) const;
            void dropDecoded(std::uint32_t inEndLevel) const;
        };

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
        static MipChain makeMipChain(
            const Image& inSource, std::uint32_t inMaxSize = MAX_SIZE, bool inIsNormal = false
        );
        static MipChain makeMipChain(
            const Pixel*  inPixels,
            int           inWidth,
            int           inHeight,
            int           inChannel,
            std::uint32_t inMaxSize  = MAX_SIZE,
            bool          inIsNormal = false
        );

    public:
        Image(const FileSystem::Path& inLocation);
        Image(const Raw& inData, ImageVendor inVendor);
        Image(const Pixel* inPixels, int inWidth, int inHeight, int inChannel, int inFormat);
        Image();

        virtual ~Image();

    public:
        ImageVendor getVendor() const;
        int getFrameCount() const;
        int getDelay(int inFrame = 0) const;
        const Pixels getPixels() const;
        const Pixels getPixels(int inFrame) const;
        void blit(Pixels outPixels, int outWidth, int outHeight, int inFrame = 0) const;
        Raw encode() const;

        std::uint32_t getMemorySize() const;

        void flipHorizontally();
        void flipVertically();
        void rotateBy(float inAngle);

    protected:
        void decode(const Raw& inData);
        void decodeGif(const Raw& inData);
        int getFrameStride() const;

    protected:
        ImageVendor      m_vendor;
        Pixels           m_pixels;
        int              m_frameCount;
        std::vector<int> m_delays;
    };
}