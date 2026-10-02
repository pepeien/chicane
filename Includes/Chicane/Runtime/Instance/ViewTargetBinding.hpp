#pragma once

#include <cstdint>

namespace Chicane
{
    class CView;

    struct CHICANE_RUNTIME InstanceViewTargetBinding
    {
    public:
        CView*        view   = nullptr;
        std::uint32_t width  = 0;
        std::uint32_t height = 0;
    };
}
