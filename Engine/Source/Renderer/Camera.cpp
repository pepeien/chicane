#include "Camera.hpp"

namespace Chicane
{
    namespace Camera
    {
        Instance::Instance()
        :
          m_ubo({}),
          m_eyes({ -550.0f, 0.0f, 100.0f }),
          m_look({ 0.0f, 0.0f, 0.0f }),
          m_up({ 0.0f, 0.0f, 1.0f }),
          m_width(0),
          m_height(0)
        {}

        glm::vec3 Instance::getPosition()
        {
            return m_eyes;
        }

        void Instance::updatePosition(const glm::vec3& inPosition)
        {
            m_eyes = inPosition;

            updateUBO();
        }

        void Instance::updateResolution(uint32_t inWidth, uint32_t inHeight)
        {
            m_width  = inWidth;
            m_height = inHeight;

            updateUBO();
        }

        UniformBufferObject Instance::getUniformBufferObject()
        {
            return m_ubo;
        }

        glm::mat4 Instance::generateView()
        {
            return glm::lookAt(
                m_eyes,
                m_look,
                m_up
            );
        }

        glm::mat4 Instance::generateProjection()
        {
            glm::mat4 result = glm::perspective(
                glm::radians(45.0f),
                static_cast<float>(m_width / m_height),
                0.1f,
                5000.0f
            );
            // Normalize OpenGL's to coordinate system Vulkan
            result[1][1] *= -1;

            return result;
        }

        void Instance::updateUBO()
        {
            m_ubo.view           = generateView();
            m_ubo.projection     = generateProjection();
            m_ubo.viewProjection = m_ubo.projection * m_ubo.view;
        }
    }
}