#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FileDialog : Dialog
        {
            const SDL_DialogFileFilter* filters     = nullptr;
            std::uint32_t               filterCount = 0;
        };
    }
}