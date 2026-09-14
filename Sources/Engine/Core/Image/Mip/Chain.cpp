#include "Chicane/Core/Image/Mip/Chain.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "Chicane/Core/Image.hpp"

namespace Chicane
{
    static void downsampleBox(
        const Image::Pixel* inSource,
        int                 inSourceWidth,
        int                 inSourceHeight,
        int                 inSourceChannel,
        Image::Pixel*       outPixels,
        int                 inWidth,
        int                 inHeight,
        bool                inIsNormal
    )
    {
        const int srcChannel = inSourceChannel > 0 ? inSourceChannel : 4;

        for (int y = 0; y < inHeight; y++)
        {
            const int srcY0 = (y * inSourceHeight) / inHeight;
            const int srcY1 = std::max(srcY0 + 1, ((y + 1) * inSourceHeight) / inHeight);

            for (int x = 0; x < inWidth; x++)
            {
                const int srcX0 = (x * inSourceWidth) / inWidth;
                const int srcX1 = std::max(srcX0 + 1, ((x + 1) * inSourceWidth) / inWidth);

                std::array<std::uint32_t, 4> acc   = {0, 0, 0, 0};
                std::uint32_t                count = 0;

                for (int srcY = srcY0; srcY < srcY1 && srcY < inSourceHeight; srcY++)
                {
                    for (int srcX = srcX0; srcX < srcX1 && srcX < inSourceWidth; srcX++)
                    {
                        const Image::Pixel* source =
                            inSource + (static_cast<std::size_t>(srcY) * inSourceWidth + srcX) * srcChannel;

                        acc.at(0) += source[0];
                        acc.at(1) += srcChannel > 1 ? source[1] : source[0];
                        acc.at(2) += srcChannel > 2 ? source[2] : source[0];
                        acc.at(3) += srcChannel > 3 ? source[3] : 255;
                        count++;
                    }
                }

                std::array<Image::Pixel, 4> pixel = {0, 0, 0, 255};
                if (count > 0)
                {
                    pixel.at(0) = static_cast<Image::Pixel>(acc.at(0) / count);
                    pixel.at(1) = static_cast<Image::Pixel>(acc.at(1) / count);
                    pixel.at(2) = static_cast<Image::Pixel>(acc.at(2) / count);
                    pixel.at(3) = static_cast<Image::Pixel>(acc.at(3) / count);
                }

                if (inIsNormal && count > 0)
                {
                    float       nx     = pixel.at(0) / 255.0f * 2.0f - 1.0f;
                    float       ny     = pixel.at(1) / 255.0f * 2.0f - 1.0f;
                    float       nz     = pixel.at(2) / 255.0f * 2.0f - 1.0f;
                    const float length = std::sqrt(nx * nx + ny * ny + nz * nz);
                    if (length > 1.0e-6f)
                    {
                        nx /= length;
                        ny /= length;
                        nz /= length;
                    }

                    pixel.at(0) = static_cast<Image::Pixel>(std::clamp(nx * 0.5f + 0.5f, 0.0f, 1.0f) * 255.0f + 0.5f);
                    pixel.at(1) = static_cast<Image::Pixel>(std::clamp(ny * 0.5f + 0.5f, 0.0f, 1.0f) * 255.0f + 0.5f);
                    pixel.at(2) = static_cast<Image::Pixel>(std::clamp(nz * 0.5f + 0.5f, 0.0f, 1.0f) * 255.0f + 0.5f);
                }

                Image::Pixel* destination = outPixels + (static_cast<std::size_t>(y) * inWidth + x) * 4;
                std::copy(pixel.begin(), pixel.end(), destination);
            }
        }
    }

    static Image::Instance makeRgbaImage(
        const Image::Pixel* inPixels, int inWidth, int inHeight, int inChannel, bool inIsNormal
    )
    {
        if (!inPixels || inWidth <= 0 || inHeight <= 0)
        {
            return {};
        }

        std::vector<Image::Pixel> pixels(static_cast<std::size_t>(inWidth) * inHeight * 4);
        downsampleBox(inPixels, inWidth, inHeight, inChannel, pixels.data(), inWidth, inHeight, inIsNormal);

        return std::make_shared<Image>(pixels.data(), inWidth, inHeight, 4, 4);
    }

    static Image::Instance downsampleImage(const Image& inSource, int inWidth, int inHeight, bool inIsNormal)
    {
        if (inWidth <= 0 || inHeight <= 0 || !inSource.getPixels())
        {
            return {};
        }

        std::vector<Image::Pixel> pixels(static_cast<std::size_t>(inWidth) * inHeight * 4);
        downsampleBox(
            inSource.getPixels(),
            inSource.getWidth(),
            inSource.getHeight(),
            inSource.getChannel(),
            pixels.data(),
            inWidth,
            inHeight,
            inIsNormal
        );

        return std::make_shared<Image>(pixels.data(), inWidth, inHeight, 4, 4);
    }

    bool ImageMipChain::isEmpty() const
    {
        return levels.empty() || (getWidth() == 0 && getHeight() == 0);
    }

    std::uint32_t ImageMipChain::getWidth() const
    {
        return levels.empty() ? 0 : static_cast<std::uint32_t>(std::max(0, levels.front().width));
    }

    std::uint32_t ImageMipChain::getHeight() const
    {
        return levels.empty() ? 0 : static_cast<std::uint32_t>(std::max(0, levels.front().height));
    }

    std::uint32_t ImageMipChain::getCount() const
    {
        return static_cast<std::uint32_t>(levels.size());
    }

    std::uint32_t ImageMipChain::streamTailMinMip(std::uint32_t inTail) const
    {
        return Image::streamTailMinMip(getWidth(), getHeight(), inTail);
    }

    std::size_t ImageMipChain::residentBytes(std::uint32_t inMinMip) const
    {
        return Image::mipChainBytes(
            Image::mipDimension(getWidth(), inMinMip),
            Image::mipDimension(getHeight(), inMinMip)
        );
    }

    Image::Instance ImageMipChain::decode(std::uint32_t inLevel) const
    {
        if (inLevel >= levels.size())
        {
            return {};
        }

        const ImageMip& mip = levels[inLevel];
        if (mip.decoded)
        {
            return mip.decoded;
        }

        if (!mip.encoded.empty())
        {
            mip.decoded = std::make_shared<Image>(mip.encoded, ImageVendor::Png);

            return mip.decoded;
        }

        if (inLevel == 0)
        {
            return {};
        }

        const Image::Instance parent = decode(inLevel - 1);
        if (!parent)
        {
            return {};
        }

        mip.decoded = downsampleImage(*parent, mip.width, mip.height, bNormal);

        return mip.decoded;
    }

    void ImageMipChain::ensureDecoded(std::uint32_t inMinLevel) const
    {
        if (levels.empty())
        {
            return;
        }

        const std::uint32_t minLevel = std::min(inMinLevel, getCount() - 1);
        for (std::uint32_t level = minLevel; level < getCount(); level++)
        {
            decode(level);
        }

        dropDecoded(minLevel);
    }

    void ImageMipChain::dropDecoded(std::uint32_t inEndLevel) const
    {
        const std::uint32_t end = std::min(inEndLevel, getCount());
        for (std::uint32_t level = 0; level < end; level++)
        {
            levels.at(level).decoded.reset();
        }
    }

    ImageMipChain Image::makeMipChain(const Image& inSource, std::uint32_t inMaxSize, bool inIsNormal)
    {
        return makeMipChain(
            inSource.getPixels(),
            inSource.getWidth(),
            inSource.getHeight(),
            inSource.getChannel(),
            inMaxSize,
            inIsNormal
        );
    }

    ImageMipChain Image::makeMipChain(
        const Pixel* inPixels, int inWidth, int inHeight, int inChannel, std::uint32_t inMaxSize, bool inIsNormal
    )
    {
        ImageMipChain chain;
        chain.bNormal = inIsNormal;

        if (!inPixels || inWidth <= 0 || inHeight <= 0)
        {
            return chain;
        }

        const std::uint32_t maxSize = std::max(1u, inMaxSize);
        const int width  = static_cast<int>(std::min(maxSize, floorPowerOfTwo(static_cast<std::uint32_t>(inWidth))));
        const int height = static_cast<int>(std::min(maxSize, floorPowerOfTwo(static_cast<std::uint32_t>(inHeight))));

        Instance current;
        if (width == inWidth && height == inHeight && inChannel == 4 && !inIsNormal)
        {
            current = std::make_shared<Image>(inPixels, inWidth, inHeight, inChannel, inChannel);
        }
        else if (width == inWidth && height == inHeight)
        {
            current = makeRgbaImage(inPixels, inWidth, inHeight, inChannel, inIsNormal);
        }
        else
        {
            std::vector<Pixel> pixels(static_cast<std::size_t>(width) * height * 4);
            downsampleBox(inPixels, inWidth, inHeight, inChannel, pixels.data(), width, height, inIsNormal);
            current = std::make_shared<Image>(pixels.data(), width, height, 4, 4);
        }

        if (!current)
        {
            return chain;
        }

        const std::uint32_t count = mipCount(static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height));
        chain.levels.resize(count);

        for (std::uint32_t level = 0; level < count; level++)
        {
            ImageMip& mip = chain.levels[level];
            mip.width     = static_cast<int>(mipDimension(static_cast<std::uint32_t>(width), level));
            mip.height    = static_cast<int>(mipDimension(static_cast<std::uint32_t>(height), level));

            if (level > 0)
            {
                current = downsampleImage(*current, mip.width, mip.height, inIsNormal);
                if (!current)
                {
                    chain.levels.resize(level);

                    break;
                }
            }

            mip.decoded = current;
            mip.encoded = current->encode();
        }

        return chain;
    }
}
