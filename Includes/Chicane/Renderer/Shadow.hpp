#pragma once

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Light.hpp"
#include "Chicane/Renderer/Shadow/Light.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Shadow
        {
            CHICANE_RENDERER ShadowLight build(const View& inCamera, const Light::List& inLights);
        }
    }
}
