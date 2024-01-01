#include "Camera.hpp"

namespace Chicane
{
    namespace Camera
    {
        Instance::Instance(float inWidth, float inHeight)
        : m_eyes({ 0.0f, 0.0f, 0.0f }),
          m_look({ 1.0f, 0.0f, 1.0f }),
          m_up({ 0.0f, 0.0f, 1.0f }),
          m_width(inWidth),
          m_height(inHeight)
        {}

        void Instance::updatePosition(const glm::vec3& inPosition)
        {
            m_eyes = inPosition;

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
                m_width / m_height,
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