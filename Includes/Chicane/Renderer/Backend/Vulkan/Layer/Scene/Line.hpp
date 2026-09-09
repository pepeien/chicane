#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLSceneLine : public Layer
        {
        public:
            VulkanLSceneLine();

        protected:
            void onInit() override;
            void onRestart() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData = nullptr) override;

        private:
            void initFrameResources();
            void destroyFrameResources();

            void initMeshGraphicsPipeline();
            void initOverlayGraphicsPipeline();
            void initFramebuffers();

            void ensureOverlayBuffer(std::size_t inVertexCount);
            void destroyOverlayBuffer();
            void uploadOverlayBuffer(const Vertex::List& inVertices);

            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawOverlay() const;

        private:
            VulkanGraphicsPipeline      m_meshPipeline;
            VulkanGraphicsPipeline      m_overlayPipeline;

            VulkanDescriptorBundle      m_frameDescriptor;

            VulkanBuffer                m_overlayBuffer;
            std::size_t                 m_overlayBufferCapacity;
            std::uint32_t               m_overlayVertexCount;

            std::vector<vk::ClearValue> m_clear;
        };
    }
}
