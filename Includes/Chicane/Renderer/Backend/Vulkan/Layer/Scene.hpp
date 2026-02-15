#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLScene : public Layer
        {
        public:
            static constexpr inline const char* ID = "Engine_Scene";

        public:
            VulkanLScene();
            ~VulkanLScene();

        protected:
            bool onInit() override;
            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

        private:
            // Model
            void buildModelVertexBuffer();
            void buildModelVertexData(const Vertex::List& inVertices);
            void buildModelIndexBuffer();
            void buildModelIndexData(const Vertex::Indices& inIndices);
            void destroyModelData();

            // Layer
            void buildLayers();

        public:
            VulkanBuffer modelVertexBuffer;
            VulkanBuffer modelIndexBuffer;
        };
    }
}