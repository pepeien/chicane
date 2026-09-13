#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanBackend;
        class VulkanFrame;

        class CHICANE_RENDERER VulkanBloomPass
        {
        public:
            void init(VulkanBackend* inBackend);
            void destroy();
            void rebuildFramebuffers();
            void apply(VulkanFrame& inGpuFrame, const Frame& inFrame, bool bInShouldPresentToWindow);

        private:
            void initPipelines();
            void destroyPipelines();

            VulkanBackend*         m_backend = nullptr;
            VulkanGraphicsPipeline m_extract;
            VulkanGraphicsPipeline m_blur;
            VulkanGraphicsPipeline m_composite;
            VulkanDescriptorBundle m_extractDescriptor;
            VulkanDescriptorBundle m_blurDescriptor;
            VulkanDescriptorBundle m_compositeDescriptor;
        };
    }
}
