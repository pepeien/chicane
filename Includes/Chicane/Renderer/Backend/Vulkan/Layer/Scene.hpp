#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLScene : public Layer
        {
        public:
            VulkanLScene();

        protected:
            void onInit() override;
            void onDestruction() override;

            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

        private:
            // Model
            void buildModelVertexBuffer();
            void buildModelVertexData(const Vertex::List& inVertices);
            void buildModelIndexBuffer();
            void buildModelIndexData(const Vertex::Indices& inIndices);
            void destroyModelData();

            // Images
            void buildShadowImage();
            void destroyShadowImage();

            // Layer
            void buildLayers();

        public:
            VulkanBuffer            modelVertexBuffer;
            VulkanBuffer            modelIndexBuffer;

            VulkanImageData         shadowImage;
            vk::DescriptorImageInfo shadowImageInfo;
        };
    }
}