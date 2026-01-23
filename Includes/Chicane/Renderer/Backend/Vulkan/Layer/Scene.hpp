#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLScene : public Layer
        {
        public:
            VulkanLScene();
            ~VulkanLScene();

        protected:
            bool onInit() override;
            void onLoad(const DrawTexture::List& inResources) override;
            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;
            bool onSetup(const Frame& inFrame) override;

        private:
            // Texture
            void buildTextureDescriptor();
            void buildTextureData(const DrawTexture::List& inTextures);
            void destroyTextureData();

            // Model
            void buildModelVertexBuffer();
            void buildModelVertexData(const Vertex::List& inVertices);
            void buildModelIndexBuffer();
            void buildModelIndexData(const Vertex::Indices& inIndices);
            void destroyModelData();

        public:
            VulkanDescriptorBundle                      textureDescriptor;
            std::vector<std::unique_ptr<VulkanTexture>> textures;

            VulkanBuffer                                modelVertexBuffer;
            VulkanBuffer                                modelIndexBuffer;
        };
    }
}