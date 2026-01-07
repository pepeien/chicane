#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Window/Type.hpp"
#include "Chicane/Core/Window/Renderer.hpp"

namespace Chicane
{
    struct CHICANE_CORE WindowCreateInfo
    {
    public:
        String           title    = "";
        FileSystem::Path icon     = "";
        Vec<2, int>      size     = Vec<2, int>(0);
        std::uint32_t    display  = 0;
        WindowType       type     = WindowType::Windowed;
        WindowRenderer   renderer = WindowRenderer::Undefined;
    };
}