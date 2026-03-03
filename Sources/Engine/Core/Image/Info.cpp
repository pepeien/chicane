#include "Chicane/Core/Image/Info.hpp"

namespace Chicane
{
    int ImageInfo::getWidth() const
    {
        return m_width;
    }

    int ImageInfo::getHeight() const
    {
        return m_height;
    }

    int ImageInfo::getChannel() const
    {
        return m_channel;
    }

    int ImageInfo::getFormat() const
    {
        return m_format;
    }

    int ImageInfo::getPitch() const
    {
        return m_format * m_width;
    }

    int ImageInfo::getSize() const
    {
        return m_width * m_height;
    }
}