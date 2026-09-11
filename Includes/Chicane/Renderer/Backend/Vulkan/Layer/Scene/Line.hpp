#pragma once

#include <vector>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line/OverlayBuffer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanFrame;

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
            void initOverlayFillGraphicsPipeline();
            void initOutlineGraphicsPipeline();
            void initFramebuffers();

            void drawOutlineMeshes(
                const Frame&       inFrame,
                vk::CommandBuffer  inCommandBuffer,
                vk::PipelineLayout inLayout,
                float              inOffsetX,
                float              inOffsetY
            ) const;

            VulkanLSceneLineOverlayBuffer& overlayBufferFor(VulkanFrame& inFrame);
            void                           ensureOverlayBuffer(
                                              VulkanLSceneLineOverlayBuffer& outBuffer, std::size_t inVertexCount
                                          );
            void                           destroyOverlayBuffer(VulkanLSceneLineOverlayBuffer& inBuffer);
            void                           destroyOverlayBuffers();
            void                           uploadOverlayBuffer(
                                              VulkanLSceneLineOverlayBuffer& outBuffer, const Vertex::List& inVertices
                                          );

            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawOverlay(const Frame& inFrame) const;
            bool shouldDrawOutline(const Frame& inFrame) const;

        private:
            VulkanGraphicsPipeline                     m_meshPipeline;
            VulkanGraphicsPipeline                     m_overlayPipeline;
            VulkanGraphicsPipeline                     m_overlayFillPipeline;
            VulkanGraphicsPipeline                     m_outlineMaskPipeline;
            VulkanGraphicsPipeline                     m_outlinePipeline;

            VulkanDescriptorBundle                     m_frameDescriptor;

            std::vector<VulkanLSceneLineOverlayBuffer> m_overlayBuffers;

            std::vector<vk::ClearValue>                m_clear;
        };
    }
}
