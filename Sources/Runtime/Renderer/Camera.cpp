#include "Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
    : m_matrixUBO({}),
      m_vectorUBO({}),
      m_position({ -550.0f, 0.0f, 100.0f }),
      m_aim({ 0.0f, 0.0f, 0.0f }),
      m_up({ 0.0f, 0.0f, 1.0f }),
      m_width(0),
      m_height(0)
    {}

    void Camera::setPosition(const glm::vec3& inPosition)
    {
        m_position = inPosition;

        updateUBOs();
    }

    glm::vec3 Camera::getPosition()
    {
        return m_position;
    }

    void Camera::addYaw(float inYaw)
    {
        float clampedYaw = std::clamp(
            inYaw,
            -1.0f,
            1.0f
        );

        m_aim.y += clampedYaw;

        updateUBOs();
    }

    void Camera::addPitch(float inPitch)
    {
        float clampedPitch = std::clamp(
            inPitch,
            -1.0f,
            1.0f
        );

        m_aim.z += clampedPitch;

        updateUBOs();
    }

    void Camera::setResolution(uint32_t inWidth, uint32_t inHeight)
    {
        m_width  = inWidth;
        m_height = inHeight;

        updateUBOs();
    }

    VectorUBO Camera::getVectorUBO()
    {
        return m_vectorUBO;
    }

    MatrixUBO Camera::getMatrixUBO()
    {
        return m_matrixUBO;
    }

    glm::mat4 Camera::generateView()
    {
        return glm::lookAt(
            m_position,
            m_aim,
            m_up
        );
    }

    glm::mat4 Camera::generateProjection()
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

    void Camera::updateUBOs()
    {
        m_matrixUBO.view           = generateView();
        m_matrixUBO.projection     = generateProjection();
        m_matrixUBO.viewProjection = m_matrixUBO.projection * m_matrixUBO.view;
    }
}