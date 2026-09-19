#pragma once

#include <unordered_map>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Sampler.hpp"
#include "Chicane/Renderer/RHI/Scissor.hpp"
#include "Chicane/Renderer/RHI/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            class CHICANE_RENDERER Frame
            {
            public:
                using BindGroups = std::unordered_map<String, BindGroup>;

            public:
                void addBindGroup(const String& inId, BindGroup inGroup);
                BindGroup getBindGroup(const String& inId) const;

            public:
                // Commands
                CommandList*  commands = nullptr;

                // Niffers
                Buffer        cameraBuffer;
                Buffer        lightBuffer;
                Buffer        instance3DBuffer;
                Buffer        instance2DBuffer;
                Buffer        particleBuffer;

                // Textures
                BindGroup     textureTable;

                // Scene
                Image         sceneColor;
                Image         sceneDepth;

                // Present Image
                Image         presentColor;

                // Sampling
                Sampler       linearSampler;

                // Viewport / Scissor
                Viewport      viewport;
                Scissor       scissor;

                // Frame
                std::uint32_t frameIndex = 0;

                // Size
                std::uint32_t width  = 0;
                std::uint32_t height = 0;

            private:
                BindGroups m_bindGroups;
            };
        }
    }
}
