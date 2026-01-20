#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Backend.hpp"
#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    struct CHICANE_CORE WindowSettings
    {
    public:
        String           title    = "";
        FileSystem::Path icon     = "";
        Vec<2, int>      size     = Vec<2, int>(0);
        Vec<2, int>      position = Vec<2, int>(0);
        std::uint32_t    display  = 0;
        WindowType       type     = WindowType::Windowed;
        WindowBackend    backend  = WindowBackend::Undefined;
    };
}