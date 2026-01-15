#pragma once

#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        class CHICANE_RUNTIME Renderer : public Chicane::Renderer
        {
        public:
            Renderer();
            virtual ~Renderer();

        protected:
            void onInit() override;
            void onRender() override;
            void onResizing() override;

        private:
            void buildContext();
            void destroyContext();
            void buildGlew();
            void buildDebugMessenger();
            void buildLayers();

            // Camera
            void buildCameraData();
            void destroyCameraData();

            // Light
            void buildLightData();
            void destroyLightData();

            // Mesh
            void buildMeshData();
            void destroyMeshData();
            void rebuildMeshData();

        private:
            std::uint32_t m_cameraBuffer;
            std::uint32_t m_lightBuffer;
            std::uint32_t m_meshBuffer;
        };
    }
}