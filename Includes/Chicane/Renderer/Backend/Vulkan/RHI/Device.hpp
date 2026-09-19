#pragma once

#include <memory>
#include <unordered_map>

#include "Chicane/Renderer/Backend/Vulkan/RHI/BindGroup/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/BindGroup/Layout/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/Buffer/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/Image/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/Pipeline/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/RHI/Sampler/Data.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanBackend;
        class VulkanRHICommandList;

        namespace RHI
        {
            class CommandList;
        }

        class CHICANE_RENDERER VulkanRHIDevice : public RHI::Device
        {
        public:
            explicit VulkanRHIDevice(VulkanBackend* inBackend);
            ~VulkanRHIDevice() override;

            RHI::Buffer createBuffer(const RHI::BufferCreateInfo& inCreateInfo) override;
            void destroyBuffer(RHI::Buffer inBuffer) override;
            void updateBuffer(
                RHI::Buffer inBuffer, const void* inData, std::size_t inSize, std::size_t inOffset = 0
            ) override;

            RHI::Image createImage(const RHI::ImageCreateInfo& inCreateInfo) override;
            void destroyImage(RHI::Image inImage) override;
            void updateImage(
                RHI::Image    inImage,
                const void*   inData,
                std::uint32_t inWidth,
                std::uint32_t inHeight,
                std::uint32_t inLayer = 0
            ) override;
            void generateMips(RHI::Image inImage) override;
            RHI::Image createImageView(RHI::Image inImage, std::uint32_t inLayer) override;

            RHI::Sampler createSampler(const RHI::SamplerCreateInfo& inCreateInfo) override;
            void destroySampler(RHI::Sampler inSampler) override;

            RHI::BindGroupLayout createBindGroupLayout(const RHI::BindGroupLayoutCreateInfo& inCreateInfo) override;
            void destroyBindGroupLayout(RHI::BindGroupLayout inLayout) override;

            RHI::BindGroup createBindGroup(
                RHI::BindGroupLayout inLayout, const std::vector<RHI::BindResource>& inResources
            ) override;
            void updateBindGroup(RHI::BindGroup inGroup, const std::vector<RHI::BindResource>& inResources) override;
            void destroyBindGroup(RHI::BindGroup inGroup) override;

            RHI::Pipeline createPipeline(const RHI::PipelineCreateInfo& inCreateInfo) override;
            void destroyPipeline(RHI::Pipeline inPipeline) override;

            String shaderPath(const String& inLogical, ShaderType inType) const override;
            RHI::ImageFormat sceneColorFormat() const override;
            RHI::ImageFormat sceneDepthFormat() const override;
            RHI::ImageFormat presentColorFormat() const override;

            RHI::Buffer wrapBuffer(const VulkanBuffer& inBuffer, vk::DeviceSize inSize, void* inMapped);
            RHI::Image wrapImage(
                const VulkanImageInfo& inInfo,
                RHI::ImageKind         inKind,
                RHI::ImageFormat       inFormat,
                std::uint32_t          inLayers = 1
            );
            RHI::BindGroup wrapDescriptorSet(vk::DescriptorSet inSet);
            RHI::CommandList* commandList();
            void setCommandBuffer(vk::CommandBuffer inCommandBuffer);
            VulkanBackend* backend() const { return m_backend; }

            vk::Format toVkFormat(RHI::ImageFormat inFormat) const;
            vk::ImageLayout shaderLayout(RHI::ImageKind inKind) const;
            vk::ImageLayout rememberedLayout(const VulkanRHIImageData* inImage) const;
            void rememberLayout(VulkanRHIImageData* inImage, vk::ImageLayout inLayout);

        private:
            void writeGroup(VulkanRHIGroupData* inGroup, const std::vector<RHI::BindResource>& inResources);
            std::uint64_t imageKey(const VulkanRHIImageData* inImage) const;

        private:
            VulkanBackend*                                     m_backend;
            std::unique_ptr<VulkanRHICommandList>              m_commands;
            std::unordered_map<std::uint64_t, vk::ImageLayout> m_imageLayouts;
        };
    }
}
