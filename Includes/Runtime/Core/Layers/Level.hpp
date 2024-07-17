#pragma once

#include "Runtime/Runtime.hpp"

#include "Runtime/Core/Layer.hpp"

#include "Runtime/Game/Level.hpp"

#include "Runtime/Renderer.hpp"

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
        void loadActor(Actor* inActor);
        void loadAssets();
        void initFrameDescriptorSetLayout();
        void initMaterialDescriptorSetLayout();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void initMaterialResources();
        void buildAssets();

    private:
        Renderer* m_renderer;
        Level* m_level;

        std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;
        vk::DescriptorSet m_descriptorSet;

        std::unique_ptr<Mesh::Manager::Instance> m_meshManager;
        std::unique_ptr<Texture::Manager::Instance> m_textureManager;

        Descriptor::Bundle m_frameDescriptor;
        Descriptor::Bundle m_materialDescriptor;

        Buffer::Instance m_meshVertexBuffer;
        Buffer::Instance m_meshIndexBuffer;
    };
}