#pragma once

#include "Chicane/Runtime/Renderer.hpp"
#include "Chicane/Runtime/Renderer/OpenGL/Renderer/Internals.hpp"

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

        public:
            RendererInternals getInternals();

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

        private:
            GLuint m_cameraBuffer;
            GLuint m_lightBuffer;
            GLuint m_meshBuffer;
        };
    }
}