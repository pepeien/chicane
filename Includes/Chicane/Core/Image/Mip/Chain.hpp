#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Image/Mip.hpp"

namespace Chicane
{
    struct Image;

    struct CHICANE_CORE ImageMipChain
    {
    public:
        bool isEmpty() const;

        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;

        std::uint32_t getCount() const;

        std::uint32_t streamTailMinMip(std::uint32_t inTail = 128) const;

        std::size_t residentBytes(std::uint32_t inMinMip) const;

        std::shared_ptr<const Image> decode(std::uint32_t inLevel) const;
        void ensureDecoded(std::uint32_t inMinLevel) const;
        void dropDecoded(std::uint32_t inEndLevel) const;

    public:
        std::vector<ImageMip> levels  = {};
        bool                  bNormal = false;
    };
}
