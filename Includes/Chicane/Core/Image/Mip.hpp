#pragma once

#include <memory>
#include <vector>

#include "Chicane/Core.hpp"

namespace Chicane
{
    struct Image;

    struct CHICANE_CORE ImageMip
    {
    public:
        int                                  width   = 0;
        int                                  height  = 0;
        std::vector<unsigned char>           encoded = {};
        mutable std::shared_ptr<const Image> decoded = {};
    };
}
