#pragma once

#include <cstdint>

#include "Chicane/Core/Math/Vec.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Resource.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Settings
        {
        public:
            Vec<2, std::uint32_t> resolution     = Vec<2, std::uint32_t>(0U);
            std::uint32_t         framesInFlight = 2;
            ResourceBudget        resourceBudget = {
                {Resource::Scene,          0.4f       }, // VRAM 40%
                {Resource::SceneIndices,   0.4f * 0.2f}, // Scene 20%
                {Resource::SceneVertices,  0.4f * 0.6f}, // Scene 60%
                {Resource::SceneInstances, 0.4f * 0.1f}, // Scene 20%

                {Resource::Texture,        0.4f       }, // VRAM 40%

                {Resource::UI,             0.1f       }, // VRAM 10%
                {Resource::UIIndices,      0.1f * 0.2f}, // UI 20%
                {Resource::UIVertices,     0.1f * 0.6f}, // UI 60%
                {Resource::UIInstances,    0.1f * 0.1f}, // UI 20%

                {Resource::Misc,           0.15f      }  // VRAM 15%
            };
        };
    }
}