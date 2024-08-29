#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Window;

    class LevelLayer : public Layer
    {
    public:
        LevelLayer(Window* inWindow);
        ~LevelLayer();

    public:
        void build() override;
        void destroy() override;
        void rebuild() override;

        void setup(Frame::Instance& outFrame) override;
        void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void loadEvents();
        void loadMeshes();
        void loadAssets();
        void initFrameDescriptorSetLayout();
        void initMaterialDescriptorSetLayout();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void destroyFrameResources();
        void initMaterialResources();
        void destroyMaterialResources();
        void buildAssets();
        void destroyAssets();

        void tickComponents(float inDeltaTime);
        void tickActors(float inDeltaTime);
        void tickAll();

    private:
        Renderer::Internals m_internals;

        Level* m_level;

        std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;
        vk::DescriptorSet m_descriptorSet;

        Descriptor::Bundle m_frameDescriptor;
        Descriptor::Bundle m_materialDescriptor;

        Buffer::Instance m_modelVertexBuffer;
        Buffer::Instance m_modelIndexBuffer;

        Texture::Manager* m_textureManager;
        Model::Manager* m_modelManager;
    };
}