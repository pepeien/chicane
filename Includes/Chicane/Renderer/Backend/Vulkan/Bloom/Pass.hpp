#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanBackend;
        class VulkanFrame;
        struct VulkanSwapchainImage;

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
            void initFullscreenPipeline(
                VulkanGraphicsPipeline&                     outPipeline,
                const char*                                 inFragment,
                const std::vector<vk::DescriptorSetLayout>& inLayouts,
                vk::Format                                  inFormat,
                bool                                        bInHasPush
            );
            void updateSample(vk::DescriptorSet inSet, std::uint32_t inBinding, const VulkanImageInfo& inImage) const;
            void beginPass(
                vk::CommandBuffer             inCommandBuffer,
                const VulkanGraphicsPipeline& inPipeline,
                vk::Framebuffer               inFramebuffer,
                vk::Extent2D                  inExtent
            ) const;
            void blurPass(
                vk::CommandBuffer     inCommandBuffer,
                VulkanSwapchainImage& inImage,
                int                   inSource,
                int                   inDestination,
                float                 inX,
                float                 inY
            );

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
