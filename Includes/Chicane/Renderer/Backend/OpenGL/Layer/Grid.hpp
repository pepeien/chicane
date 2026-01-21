#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLGrid : public Layer
        {
        public:
            OpenGLLGrid();
            ~OpenGLLGrid();

        public:
            bool onInit() override;
            bool onSetup(const Frame& inFrame, const DrawResource::Map& inResources) override;
            void onRender(const Frame& inFrame, const DrawResource::Map& inResources) override;
            void onCleanup() override;

        private:
            // Shader
            void buildShader();
            void destroyShader();

            // Component
            void buildPrimitiveVertexArray();
            void buildPrimitiveVertexBuffer();
            void buildPrimitiveIndexBuffer();
            void destroyPrimitiveData();

            // Size
            void buildInstanceData();
            void destroyInstanceData();

        private:
            // Shader
            std::uint32_t m_shaderProgram;

            // Primitive
            std::uint32_t m_primitiveVertexArray;
            std::uint32_t m_primitiveVertexBuffer;
            std::uint32_t m_primitiveIndexBuffer;

            // Instance
            std::uint32_t m_instanceBuffer;
        };
    }
}