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
        Image();

        virtual ~Image();

    public:
        ImageVendor getVendor() const;
        const Pixels getPixels() const;

        std::uint32_t getMemorySize() const;

        void flipHorizontally();
        void flipVertically();
        void rotateBy(float inAngle);

    protected:
        ImageVendor m_vendor;
        Pixels      m_pixels;
    };
}