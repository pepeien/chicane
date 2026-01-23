#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Layer.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLGrid : public Layer
        {
        public:
            VulkanLGrid(VulkanBackend* inBackend);
            ~VulkanLGrid();

        protected:
            bool onInit() override;
            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;
            bool onRebuild() override;
            bool onSetup(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            bool onDestroy() override;

        private:
            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

            // Primitive
            void buildPrimtiveVertexBuffer();
            void buildPrimitiveVertexData(const Vertex::List& inVertices);
            void buildPrimtiveIndexBuffer();
            void buildPrimitiveIndexData(const Vertex::Indices& inIndices);
            void destroyPrimitiveData();

        private:
            VulkanBackend*                          m_backend;

            std::unique_ptr<VulkanGraphicsPipeline> m_graphicsPipeline;

            VulkanDescriptorBundle                  m_frameDescriptor;

            VulkanBuffer                            m_primitiveVertexBuffer;
            VulkanBuffer                            m_primitiveIndexBuffer;

            std::vector<vk::ClearValue>             m_clear;
        };
    }
}