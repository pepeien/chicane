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

        inline String toString(BlendingFactor inValue)
        {
            switch (inValue)
            {
            case BlendingFactor::Zero:
                return "Zero";

            case BlendingFactor::One:
                return "One";

            case BlendingFactor::SrcColor:
                return "SrcColor";

            case BlendingFactor::OneMinusSrcColor:
                return "OneMinusSrcColor";

            case BlendingFactor::DstColor:
                return "DstColor";

            case BlendingFactor::OneMinusDstColor:
                return "OneMinusDstColor";

            case BlendingFactor::SrcAlpha:
                return "SrcAlpha";

            case BlendingFactor::OneMinusSrcAlpha:
                return "OneMinusSrcAlpha";

            case BlendingFactor::DstAlpha:
                return "DstAlpha";

            case BlendingFactor::OneMinusDstAlpha:
                return "OneMinusDstAlpha";

            case BlendingFactor::ConstantColor:
                return "ConstantColor";

            case BlendingFactor::OneMinusConstantColor:
                return "OneMinusConstantColor";

            case BlendingFactor::ConstantAlpha:
                return "ConstantAlpha";

            case BlendingFactor::OneMinusConstantAlpha:
                return "OneMinusConstantAlpha";

            case BlendingFactor::SrcAlphaSaturate:
                return "SrcAlphaSaturate";

            case BlendingFactor::Src1Color:
                return "Src1Color";

            case BlendingFactor::OneMinusSrc1Color:
                return "OneMinusSrc1Color";

            case BlendingFactor::Src1Alpha:
                return "Src1Alpha";

            case BlendingFactor::OneMinusSrc1Alpha:
                return "OneMinusSrc1Alpha";

            default:
                return "";
            }
        }
    }
}