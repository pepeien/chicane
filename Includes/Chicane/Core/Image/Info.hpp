#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    struct CHICANE_CORE ImageInfo
    {
    public:
        int getWidth() const;
        int getHeight() const;
        int getChannel() const;
        int getFormat() const;

        int getPitch() const;
        int getSize() const;

    protected:
        int m_width;
        int m_height;
        int m_channel;
        int m_format;
    };
}