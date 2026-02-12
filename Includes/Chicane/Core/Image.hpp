#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    struct CHICANE_CORE Image
    {
    public:
        using Raw    = std::vector<unsigned char>;
        using Pixel  = unsigned char;
        using Pixels = unsigned char*;
        using List   = std::vector<Image>;

    public:
        static ImageVendor extractVendor(const String& inValue);
        static String extractVendor(ImageVendor inValue);

    public:
        Image(const FileSystem::Path& inFilepath);
        Image(const Raw& inData, ImageVendor inVendor);
        Image();

        Image(const Image& inInstance);

        virtual ~Image();

    public:
        ImageVendor getVendor() const;

        int getWidth() const;
        int getHeight() const;
        int getChannel() const;
        int getFormat() const;
        const Pixels getPixels() const;

        int getPitch() const;
        int getSize() const;

        void rotate(float inAngle);

    protected:
        ImageVendor m_vendor;

        int         m_width;
        int         m_height;
        int         m_channel;
        int         m_format;
        Pixels      m_pixels;
    };
}