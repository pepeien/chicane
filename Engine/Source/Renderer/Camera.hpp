#pragma once

#include "Base.hpp"
#include "Buffer.hpp"

namespace Engine
{
    namespace Camera
    {
        struct MatrixUBO
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 viewProjection;
        };

        struct VectorUBO
        {
            glm::vec4 forward = { 1.0f, 0.0f, 0.0f, 0.0f };
            glm::vec4 right   = { 0.0f, -1.0f, 0.0f, 0.0f };
            glm::vec4 up      = { 0.0f, 0.0f, 1.0f, 0.0f };
        };

        struct UBOBundle
        {
            size_t allocationSize;
            void* writeLocation;
            Buffer::Instance buffer;
        };

        struct MatrixUBOBundle : public UBOBundle
        {
            MatrixUBO instance;
        };

        struct VectorUBOBundle : public UBOBundle
        {
            VectorUBO instance;
        };

        class Instance
        {
        public:
            Instance();

        public:
            glm::vec3 getPosition();
            void updatePosition(const glm::vec3& inPosition);
            
            void updateResolution(uint32_t inWidth, uint32_t inHeight);

            VectorUBO getVectorUBO();
            MatrixUBO getMatrixUBO();

        private:
            glm::mat4 generateView();
            glm::mat4 generateProjection();

            void updateUBOs();

        private:
            MatrixUBO m_matrixUBO;
            VectorUBO m_vectorUBO;

            glm::vec3 m_eyes;
            glm::vec3 m_look;
            glm::vec3 m_up;

            uint32_t m_width;
            uint32_t m_height;
        };
    }
}