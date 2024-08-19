#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
        : m_transform({}),
        m_viewportWidth(1280),
        m_viewportHeight(720),
        m_fov(45.0f),
        m_aspectRatio(1.77f),
        m_nearClip(0.2f),
        m_farClip(50000.0f),
        m_moveDistance(0.7f),
        m_UBO({})
    {}

    void Camera::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        m_viewportWidth  = inWidth;
        m_viewportHeight = inHeight;
        m_aspectRatio    = 0;

        if (inWidth == 0 || inHeight == 0)
        {
            return;
        }

        setAspectRatio(
            static_cast<float>(m_viewportWidth / m_viewportHeight)
        );
    }

    void Camera::setViewport(const Vec<std::uint32_t>::Two& inViewportResolution)
    {
        setViewport(
            inViewportResolution.x,
            inViewportResolution.y
        );
    }

    const Vec<float>::Three& Camera::getTranslation()
    {
        return m_transform.translation;
    }

    void Camera::addTranslation(const Vec<float>::Three& inPosition)
    {
        addTranslation(
            inPosition.x,
            inPosition.y,
            inPosition.z
        );
    }

    void Camera::addTranslation(float inX, float inY, float inZ)
    {
        m_transform.translation += inX;
        m_transform.translation += inY;
        m_transform.translation += inZ;

        onTransformUpdate();
    }

    void Camera::setTranslation(const Vec<float>::Three& inPosition)
    {
        m_transform.translation = inPosition;

        onTransformUpdate();
    }

    const Vec<float>::Three& Camera::getRotation()
    {
        return m_transform.rotation;
    }

    void Camera::addRotation(const Vec<float>::Three& inRotation)
    {
        addRotation(
            inRotation.x,
            inRotation.y,
            inRotation.z
        );
    }

    void Camera::addRotation(float inRoll, float inYaw, float inPitch)
    {
        m_transform.rotation.x += inRoll;
        m_transform.rotation.y += inYaw;
        m_transform.rotation.z += inPitch;

        onTransformUpdate();
    }

    void Camera::setRotation(const Vec<float>::Three& inRotation)
    {
        m_transform.rotation = inRotation;

        onTransformUpdate();
    }

    const Vec<float>::Three& Camera::getForward()
    {
        return m_forward;
    }

    const Vec<float>::Three& Camera::getRight()
    {
        return m_right;
    }

    const Vec<float>::Three& Camera::getUp()
    {
        return m_up;
    }

    const Camera::UBO& Camera::getUBO()
    {
        return m_UBO;
    }

    // TODO Implement roll
    void Camera::onTransformUpdate()
    {
        float roll  = glm::radians(m_transform.rotation.x);
        float yaw   = glm::radians(m_transform.rotation.y);
        float pitch = glm::radians(m_transform.rotation.z);

        // State
        float pitchCos = cos(pitch);

        m_forward.x = pitchCos * sin(yaw);
        m_forward.y = pitchCos * cos(yaw);
        m_forward.z = sin(pitch);

        m_forward = glm::normalize(m_forward);
        m_right   = glm::normalize(glm::cross(m_forward, UP_DIRECTION));
        m_up      = glm::cross(m_right, m_forward);

        // UBO
        m_UBO.view = glm::lookAt(
            m_transform.translation,
            m_transform.translation + m_forward,
            m_up
        );

        m_UBO.projection = glm::perspective(
            glm::radians(m_fov),
            m_aspectRatio,
            m_nearClip,
            m_farClip
        );
        // Normalize OpenGL's to coordinate system Vulkan
        m_UBO.projection[1][1] *= -1;

        m_UBO.viewProjection = m_UBO.view * m_UBO.projection;

        m_UBO.forward = Vec<float>::Four(m_forward, 0.0f);
        m_UBO.right   = Vec<float>::Four(m_right, 0.0f);
        m_UBO.up      = Vec<float>::Four(m_up, 0.0f);
    }

    void Camera::setAspectRatio(float inAspectRatio)
    {
        m_aspectRatio = inAspectRatio;
    }
}