#include "Chicane/Core/Image.hpp"

#include <algorithm>

namespace Chicane
{
    std::uint32_t Image::floorPowerOfTwo(std::uint32_t inValue)
    {
        if (inValue <= 1)
        {
            return 1;
        }

        std::uint32_t result = 1;
        while ((result << 1) <= inValue && (result << 1) != 0)
        {
            result <<= 1;
        }

        return result;
    }

    std::uint32_t Image::mipCount(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        std::uint32_t count   = 1;
        std::uint32_t largest = std::max(inWidth, inHeight);
        while (largest > 1)
        {
            largest >>= 1;
            count++;
        }

        return count;
    }

    std::uint32_t Image::mipDimension(std::uint32_t inSize, std::uint32_t inLevel)
    {
        return std::max(1u, inSize >> inLevel);
    }

    std::uint32_t Image::streamTailMinMip(std::uint32_t inWidth, std::uint32_t inHeight, std::uint32_t inTail)
    {
        const std::uint32_t tail   = std::max(1u, inTail);
        const std::uint32_t count  = mipCount(inWidth, inHeight);
        std::uint32_t       mip    = 0;
        std::uint32_t       width  = inWidth;
        std::uint32_t       height = inHeight;

        while (mip + 1 < count && (width > tail || height > tail))
        {
            width  = std::max(1u, width / 2);
            height = std::max(1u, height / 2);
            mip++;
        }

        return mip;
    }

    std::size_t Image::mipChainBytes(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        std::size_t   bytes  = 0;
        std::uint32_t width  = std::max(1u, inWidth);
        std::uint32_t height = std::max(1u, inHeight);

        while (true)
        {
            bytes += static_cast<std::size_t>(width) * height * 4u;
            if (width == 1 && height == 1)
            {
                break;
            }

            width  = std::max(1u, width / 2);
            height = std::max(1u, height / 2);
        }

        return bytes;
    }
}
