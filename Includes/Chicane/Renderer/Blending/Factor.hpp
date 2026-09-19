#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Renderer::BlendingFactor inValue)
    {
        switch (inValue)
        {
        case Renderer::BlendingFactor::Zero:
            return "Zero";

        case Renderer::BlendingFactor::One:
            return "One";

        case Renderer::BlendingFactor::SrcColor:
            return "SrcColor";

        case Renderer::BlendingFactor::OneMinusSrcColor:
            return "OneMinusSrcColor";

        case Renderer::BlendingFactor::DstColor:
            return "DstColor";

        case Renderer::BlendingFactor::OneMinusDstColor:
            return "OneMinusDstColor";

        case Renderer::BlendingFactor::SrcAlpha:
            return "SrcAlpha";

        case Renderer::BlendingFactor::OneMinusSrcAlpha:
            return "OneMinusSrcAlpha";

        case Renderer::BlendingFactor::DstAlpha:
            return "DstAlpha";

        case Renderer::BlendingFactor::OneMinusDstAlpha:
            return "OneMinusDstAlpha";

        case Renderer::BlendingFactor::ConstantColor:
            return "ConstantColor";

        case Renderer::BlendingFactor::OneMinusConstantColor:
            return "OneMinusConstantColor";

        case Renderer::BlendingFactor::ConstantAlpha:
            return "ConstantAlpha";

        case Renderer::BlendingFactor::OneMinusConstantAlpha:
            return "OneMinusConstantAlpha";

        case Renderer::BlendingFactor::SrcAlphaSaturate:
            return "SrcAlphaSaturate";

        case Renderer::BlendingFactor::Src1Color:
            return "Src1Color";

        case Renderer::BlendingFactor::OneMinusSrc1Color:
            return "OneMinusSrc1Color";

        case Renderer::BlendingFactor::Src1Alpha:
            return "Src1Alpha";

        case Renderer::BlendingFactor::OneMinusSrc1Alpha:
            return "OneMinusSrc1Alpha";

        default:
            return "";
        }
    }
}
