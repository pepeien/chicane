#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/FileSystem/Dialog.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct FileDialog : Dialog
        {
            const SDL_DialogFileFilter* filters     = nullptr;
            std::uint32_t               filterCount = 0;
        };
    }
}