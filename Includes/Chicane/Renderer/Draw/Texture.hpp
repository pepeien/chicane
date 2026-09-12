#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawTexture : public Draw
        {
        public:
            using List = std::vector<DrawTexture>;

        public:
            static const DrawTexture& empty()
            {
                static const DrawTexture result;

                return result;
            }

        public:
            Image::Instance                  image;
            std::shared_ptr<Image::MipChain> mips;
            std::uint32_t                    width           = 0;
            std::uint32_t                    height          = 0;
            std::uint32_t                    residentMinMip  = 0;
            std::uint32_t                    desiredMinMip   = 0;
            bool                             bStreamable     = true;

            std::uint32_t getMipCount() const
            {
                if (mips)
                {
                    return mips->getCount();
                }

                return 1;
            }

            std::uint32_t getStreamTailMinMip() const
            {
                if (!bStreamable)
                {
                    return 0;
                }

                if (mips)
                {
                    return mips->streamTailMinMip(TEXTURE_STREAM_TAIL);
                }

                return Image::streamTailMinMip(width, height, TEXTURE_STREAM_TAIL);
            }

            Image::Instance getSampleImage() const
            {
                if (mips)
                {
                    if (const Image::Instance top = mips->decode(0))
                    {
                        return top;
                    }
                }

                return image;
            }

            Image::Instance getResidentImage() const
            {
                if (mips)
                {
                    if (const Image::Instance mip = mips->decode(residentMinMip))
                    {
                        return mip;
                    }
                }

                return image;
            }

            std::size_t getResidentBytes() const
            {
                const std::uint32_t mipWidth  = Image::mipDimension(width, residentMinMip);
                const std::uint32_t mipHeight = Image::mipDimension(height, residentMinMip);

                return Image::mipChainBytes(mipWidth, mipHeight);
            }
        };
    }
}
