#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Window/Type.hpp"
#include "Chicane/Core/Window/Renderer.hpp"

namespace Chicane
{
    namespace Window
    {
        struct CHICANE_CORE CreateInfo
        {
        public:
            std::string      title    = "";
            FileSystem::Path icon     = "";
            Vec<2, int>      size     = Vec<2, int>(0);
            int              display  = 0;
            Type             type     = Type::Windowed;
            Renderer         renderer = Renderer::Undefined;
        };
    }
}