#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLBackend : public Backend
        {
        public:
            OpenGLBackend(const Instance* inRenderer);
            virtual ~OpenGLBackend();

        protected:
            void onInit() override;
            void onSetup(const Frame& inFrame) override;
            void onCleanup() override;

        private:
            // OpenGL
            void buildContext();
            void destroyContext();
            void buildGlew();
            void enableFeatures();

            // Camera
            void buildCameraData();
            void destroyCameraData();

            // Light
            void buildLightData();
            void destroyLightData();

            // Mesh
            void buildMeshData();
            void destroyMeshData();

            // Layer
            void buildLayers();

        private:
            std::uint32_t m_cameraBuffer;
            std::uint32_t m_lightBuffer;
            std::uint32_t m_meshBuffer;
        };
    }
}