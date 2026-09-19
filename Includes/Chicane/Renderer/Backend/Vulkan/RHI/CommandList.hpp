#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanRHICommandList : public RHI::CommandList
        {
        public:
            explicit VulkanRHICommandList(VulkanRHIDevice* inDevice);
            ~VulkanRHICommandList() override;

            void setCommandBuffer(vk::CommandBuffer inCommandBuffer);

            void beginPass(const RHI::PassCreateInfo& inCreateInfo) override;
            void endPass() override;
            void bindPipeline(RHI::Pipeline inPipeline) override;
            void bindGroup(std::uint32_t inSet, RHI::BindGroup inGroup) override;
            void bindVertexBuffer(RHI::Buffer inBuffer) override;
            void bindIndexBuffer(RHI::Buffer inBuffer) override;
            void pushConstants(const void* inData, std::uint32_t inSize) override;
            void draw(
                std::uint32_t inVertexCount,
                std::uint32_t inInstanceCount,
                std::uint32_t inFirstVertex,
                std::uint32_t inFirstInstance
            ) override;
            void drawIndexed(
                std::uint32_t inIndexCount,
                std::uint32_t inInstanceCount,
                std::uint32_t inFirstIndex,
                std::int32_t  inVertexOffset,
                std::uint32_t inFirstInstance
            ) override;
            void setViewport(const RHI::Viewport& inViewport) override;
            void setScissor(const RHI::Scissor& inScissor) override;
            void setLineWidth(float inWidth) override;
            void blitColor(
                RHI::Image    inSource,
                RHI::Image    inDestination,
                std::int32_t  inX,
                std::int32_t  inY,
                std::uint32_t inWidth,
                std::uint32_t inHeight
            ) override;
            void prepareShaderRead(RHI::Image inImage) override;
            void preparePresent(RHI::Image inImage) override;

        private:
            struct PassObjects
            {
                vk::RenderPass  pass        = nullptr;
                vk::Framebuffer framebuffer = nullptr;
            };

            vk::RenderPass framebufferPass(const RHI::PassCreateInfo& inCreateInfo);
            vk::Framebuffer framebuffer(const RHI::PassCreateInfo& inCreateInfo, vk::RenderPass inPass);
            void setLayout(VulkanRHIImageData* inImage, vk::ImageLayout inLayout);
            void destroyPassObjects(std::vector<PassObjects>& inObjects);
            std::uint64_t commandKey(vk::CommandBuffer inCommandBuffer) const;

        private:
            VulkanRHIDevice*                                            m_device;
            vk::CommandBuffer                                           m_commands    = nullptr;
            VulkanRHIPipelineData*                                      m_pipeline    = nullptr;
            bool                                                        m_bInPass     = false;
            vk::RenderPass                                              m_pass        = nullptr;
            vk::Framebuffer                                             m_framebuffer = nullptr;
            VulkanRHIImageData*                                         m_passColor   = nullptr;
            VulkanRHIImageData*                                         m_passDepth   = nullptr;
            std::unordered_map<std::uint64_t, std::vector<PassObjects>> m_retiredPasses;
        };
    }
}
