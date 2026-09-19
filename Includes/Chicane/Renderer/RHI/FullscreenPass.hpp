#pragma once

#include <array>
#include <vector>

#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            class CHICANE_RENDERER FullscreenPass
            {
            public:
                void init(Device* inDevice);
                void destroy();
                void resize(std::uint32_t inWidth, std::uint32_t inHeight, ImageFormat inFormat);
                void apply(
                    CommandList*  inCommands,
                    Image         inSceneColor,
                    Image         inOutput,
                    std::uint32_t inWidth,
                    std::uint32_t inHeight,
                    bool          bHdr,
                    std::uint32_t inFrameIndex
                );

                void destroyImages();

            private:
                // Device
                Device*                  m_device = nullptr;

                // Compositing
                Pipeline                 m_composite       = {};
                BindGroupLayout          m_compositeLayout = {};
                std::vector<BindGroup>   m_compositeGroups = {};

                // Sampling
                Sampler                  m_sampler      = {};
                BindGroupLayout          m_sampleLayout = {};

                // Bloom
                std::array<Image, 2>     m_bloom     = {};
                std::array<BindGroup, 2> m_blurGroup = {};

                // Blur
                Pipeline                 m_blur = {};

                // Extraction
                Pipeline                 m_extract       = {};
                std::vector<BindGroup>   m_extractGroups = {};

                // Size
                std::uint32_t            m_width  = 0;
                std::uint32_t            m_height = 0;
            };
        }
    }
}
