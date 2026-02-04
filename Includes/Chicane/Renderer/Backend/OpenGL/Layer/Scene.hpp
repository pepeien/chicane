#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLScene : public Layer<Frame>
        {
        public:
            OpenGLLScene();
            ~OpenGLLScene();

        public:
            bool onInit() override;
            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;
            bool onSetup(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            // View
            void buildCameraData();
            void destroyCameraData();

            void buildLightData();
            void destroyLightData();

            // Model
            void buildModelVertexArray();
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void destroyModelData();

            // Instance
            void buildInstanceData();
            void destroyInstanceData();

            // Layer
            void buildLayers();

        private:
            // View
            std::uint32_t m_cameraBuffer;
            std::uint32_t m_lightBuffer;

            // Model
            std::uint32_t m_modelVertexArray;
            std::uint32_t m_modelVertexBuffer;
            std::uint32_t m_modelIndexBuffer;

            // Instance
            std::uint32_t m_instanceBuffer;
        };
    }
}