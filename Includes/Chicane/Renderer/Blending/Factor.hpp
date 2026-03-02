#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class BlendingFactor : std::uint8_t
        {
            Zero,
            One,
            SrcColor,
            OneMinusSrcColor,
            DstColor,
            OneMinusDstColor,
            SrcAlpha,
            OneMinusSrcAlpha,
            DstAlpha,
            OneMinusDstAlpha,
            ConstantColor,
            OneMinusConstantColor,
            ConstantAlpha,
            OneMinusConstantAlpha,
            SrcAlphaSaturate,
            Src1Color,
            OneMinusSrc1Color,
            Src1Alpha,
            OneMinusSrc1Alpha
        };
    }
}