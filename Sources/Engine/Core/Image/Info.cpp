#include "Chicane/Core/Image/Info.hpp"

namespace Chicane
{
    ImageInfo::ImageInfo()
        : m_width(0),
          m_height(0),
          m_channel(0),
          m_format(0)
    {}

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