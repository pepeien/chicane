#pragma once

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

    public:
        static ImageVendor parseVendor(const String& inValue);
        static const String& getVendorExtension(ImageVendor inValue);

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