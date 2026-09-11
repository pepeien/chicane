#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line/ImmediateBuffer.hpp"
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
            void initLineListGraphicsPipeline(VulkanGraphicsPipeline& outPipeline, bool inTestDepth);
            void initOutlineGraphicsPipeline();
            void initFramebuffers();

            void drawOutlineMeshes(
                const Frame&       inFrame,
                vk::CommandBuffer  inCommandBuffer,
                vk::PipelineLayout inLayout,
                float              inOffsetX,
                float              inOffsetY
            ) const;

            VulkanLSceneLineImmediateBuffer& immediateBufferFor(VulkanFrame& inFrame);
            void ensureImmediateVertexBuffer(VulkanLSceneLineImmediateBuffer& outBuffer, std::size_t inBytes);
            void ensureImmediateIndexBuffer(VulkanLSceneLineImmediateBuffer& outBuffer, std::size_t inBytes);
            void destroyImmediateBuffer(VulkanLSceneLineImmediateBuffer& inBuffer);
            void destroyImmediateBuffers();
            void uploadImmediateGeometry(VulkanLSceneLineImmediateBuffer& outBuffer, const Frame& inFrame);

            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawLineList(const Frame& inFrame) const;
            bool shouldDrawOutline(const Frame& inFrame) const;

            void drawLineList(
                vk::CommandBuffer inCommandBuffer, const DrawPoly& inDraw, vk::Buffer inIndexBuffer
            ) const;

        private:
            VulkanGraphicsPipeline                       m_meshPipeline;
            VulkanGraphicsPipeline                       m_lineListPipeline;
            VulkanGraphicsPipeline                       m_lineListForegroundPipeline;
            VulkanGraphicsPipeline                       m_outlineMaskPipeline;
            VulkanGraphicsPipeline                       m_outlinePipeline;

            VulkanDescriptorBundle                       m_frameDescriptor;

            std::vector<VulkanLSceneLineImmediateBuffer> m_immediateBuffers;

            std::vector<vk::ClearValue>                  m_clear;
        };
    }
}
