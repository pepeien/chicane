#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/Layer.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Renderer
    {
        class VulkanFrame;

        class CHICANE_RENDERER VulkanLUI : public Layer
        {
        public:
            using Outlines = std::vector<float>;

        public:
            VulkanLUI();

        protected:
            void onInit() override;
            void onRestart() override;
            void onDestruction() override;

            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

            // Primitive
            void buildPrimitiveVertexBuffer();
            void buildPrimitiveVertexData(const Vertex::List& inVertices);
            void buildPrimitiveIndexBuffer();
            void buildPrimitiveIndexData(const Vertex::Indices& inIndices);
            void destroyPrimitiveData();

            // Glyph
            void buildGlyphBuffer();
            void buildGlyphData(const Outlines& inOutlines);
            void destroyGlyphData();

            // Backdrop
            void buildBackdrop();
            void destroyBackdrop();
            void copyBackdrop(VulkanFrame& inFrame, std::size_t inIndex);
            void generateBackdropMips(vk::CommandBuffer inCommands, std::size_t inIndex);

        private:
            VulkanGraphicsPipeline               m_graphicsPipeline;

            VulkanDescriptorBundle               m_frameDescriptor;

            VulkanBuffer                         m_primitiveVertexBuffer;
            VulkanBuffer                         m_primitiveIndexBuffer;

            VulkanBuffer                         m_glyphBuffer;
            vk::DescriptorBufferInfo             m_glyphBufferInfo;

            std::vector<VulkanImageInfo>         m_backdrops;
            std::vector<vk::DescriptorImageInfo> m_backdropInfos;
            std::uint32_t                        m_backdropMipLevels = 1;

            std::vector<vk::ClearValue>          m_clear;
        };
    }
}