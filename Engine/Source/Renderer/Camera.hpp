#pragma once

#include "Base.hpp"
#include "Buffer.hpp"
#include "Uniform.hpp"

namespace Chicane
{
    namespace Camera
    {
        struct UniformBufferObject
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 viewProjection;
        };

        struct Bundle
        {
            size_t allocationSize;
            void* writeLocation;
            Buffer::Instance buffer;
            UniformBufferObject object;
        };

        class Instance
        {
        public:
            Instance();

        public:
            glm::vec3 getPosition();
            void updatePosition(const glm::vec3& inPosition);
            
            void updateResolution(uint32_t inWidth, uint32_t inHeight);

            UniformBufferObject getUniformBufferObject();

        private:
            glm::mat4 generateView();
            glm::mat4 generateProjection();

            void updateUBO();

        private:
            UniformBufferObject m_ubo;

            glm::vec3 m_eyes;
            glm::vec3 m_look;
            glm::vec3 m_up;

            uint32_t m_width;
            uint32_t m_height;
        };
    }
}