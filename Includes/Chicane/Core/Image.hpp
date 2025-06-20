#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    struct CHICANE_CORE Image
    {
    public:
        using Raw    = std::vector<unsigned char>;
        using Pixels = unsigned char*;

    protected:
        using Super = Image;

    public:
        Image(const FileSystem::Path& inFilepath);
        Image(const Raw& inData);
        Image();

        Image(const Image& inInstance);

        virtual ~Image();

    public:
        int getWidth() const;
        int getHeight() const;
        int getChannel() const;
        int getFormat() const;
        const Pixels getPixels() const;

        int getPitch() const;
        int getSize() const;

    protected:
        ImageVendor m_type;

        int         m_width;
        int         m_height;
        int         m_channel;
        int         m_format;
        Pixels      m_pixels;
    };
}