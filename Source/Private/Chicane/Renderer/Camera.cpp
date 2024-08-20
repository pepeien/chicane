#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
        : m_transform({}),
        m_forward(FORWARD_DIRECTION),
        m_up(UP_DIRECTION),
        m_right(RIGHT_DIRECTION),
        m_viewport(Vec<std::uint32_t>::Two(0.0f)),
        m_aspectRatio(0.0f),
        m_fov(45.0f),
        m_nearClip(0.0f),
        m_farClip(50000.0f),
        m_UBO({})
    {
        onSettingsUpdate();
        onTransformUpdate();
    }

    const Vec<std::uint32_t>::Two& Camera::getViewport()
    {
        return m_viewport;
    }

    void Camera::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        m_viewport.x  = inWidth;
        m_viewport.y  = inHeight;
        m_aspectRatio = 0;

        if (inWidth > 0 && inHeight > 0)
        {
            m_aspectRatio = m_viewport.x / m_viewport.y;
        }

        onSettingsUpdate();
    }

    void Camera::setViewport(const Vec<std::uint32_t>::Two& inViewportResolution)
    {
        setViewport(
            inViewportResolution.x,
            inViewportResolution.y
        );
    }

    float Camera::getFov()
    {
        return m_fov;
    }

    void Camera::setFov(float inFov)
    {
        m_fov = inFov;

        onSettingsUpdate();
    }

    float Camera::getNearClip()
    {
        return m_nearClip;
    }

    void Camera::setNearClip(float inNearClip)
    {
        m_nearClip = inNearClip;

        onSettingsUpdate();
    }

    float Camera::getFarClip()
    {
        return m_farClip;
    }

    void Camera::setFarClip(float inFarClip)
    {
        m_farClip = inFarClip;

        onSettingsUpdate();
    }

    void Camera::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Vec<float>::Three& Camera::getTranslation()
    {
        return m_transform.translation;
    }

    void Camera::setTranslation(const Vec<float>::Three& inPosition)
    {
        m_transform.translation = inPosition;

        onTransformUpdate();
    }

    void Camera::addTranslation(float inX, float inY, float inZ)
    {
        m_transform.translation.x += inX;
        m_transform.translation.y += inY;
        m_transform.translation.z += inZ;

        onTransformUpdate();
    }

    void Camera::addTranslation(const Vec<float>::Three& inPosition)
    {
        addTranslation(
            inPosition.x,
            inPosition.y,
            inPosition.z
        );
    }

    const Vec<float>::Three& Camera::getRotation()
    {
        return m_transform.rotation;
    }

    void Camera::setRotation(const Vec<float>::Three& inRotation)
    {
        m_transform.rotation = inRotation;

        onTransformUpdate();
    }

    void Camera::addRotation(float inRoll, float inYaw, float inPitch)
    {
        m_transform.rotation.x += inRoll;
        m_transform.rotation.y += inYaw;
        m_transform.rotation.z += inPitch;

        onTransformUpdate();
    }

    void Camera::addRotation(const Vec<float>::Three& inRotation)
    {
        addRotation(
            inRotation.x,
            inRotation.y,
            inRotation.z
        );
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

    void Camera::onSettingsUpdate()
    {
        m_UBO.projection = glm::perspective(
            glm::radians(m_fov),
            m_aspectRatio,
            m_nearClip,
            m_farClip
        );
        // Normalize OpenGL's to Vulkan's coordinate system
        m_UBO.projection[1][1] *= -1;

        m_UBO.viewProjection = m_UBO.view * m_UBO.projection;
    }

    void Camera::onTransformUpdate()
    {
        // Apply Yaw & Pitch
        float yaw   = glm::radians(m_transform.rotation.y);
        float pitch = glm::radians(m_transform.rotation.z);

        float pitchCos = cos(pitch);

        m_forward.x = pitchCos * sin(yaw);
        m_forward.y = pitchCos * cos(yaw);
        m_forward.z = sin(pitch);

        m_forward = glm::normalize(m_forward);
        m_right   = glm::normalize(glm::cross(m_forward, UP_DIRECTION));
        m_up      = glm::cross(m_right, m_forward);

        // Apply roll
        float roll  = glm::radians(m_transform.rotation.x);

        m_right = glm::normalize(glm::rotate(m_right, roll, m_forward));
        m_up    = glm::normalize(glm::rotate(m_up, roll, m_forward));

        // UBO
        m_UBO.view = glm::lookAt(
            m_transform.translation,
            m_transform.translation + m_forward,
            m_up
        );
        m_UBO.view = glm::inverse(m_UBO.view);

        m_UBO.viewProjection = m_UBO.view * m_UBO.projection;

        m_UBO.forward = Vec<float>::Four(m_forward, 0.0f);
        m_UBO.right   = Vec<float>::Four(m_right, 0.0f);
        m_UBO.up      = Vec<float>::Four(m_up, 0.0f);
    }
}