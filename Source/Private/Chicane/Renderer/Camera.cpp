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
        m_nearClip(0.1f),
        m_farClip(50000.0f),
        m_UBO({})
    {}

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
            m_aspectRatio = (float) m_viewport.x / m_viewport.y;
        }

        updateProjection();
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

        updateProjection();
    }

    float Camera::getNearClip()
    {
        return m_nearClip;
    }

    void Camera::setNearClip(float inNearClip)
    {
        m_nearClip = inNearClip;

        updateProjection();
    }

    float Camera::getFarClip()
    {
        return m_farClip;
    }

    void Camera::setFarClip(float inFarClip)
    {
        m_farClip = inFarClip;

        updateProjection();
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
        updateTranslation(inPosition);
    }

    void Camera::addTranslation(const Vec<float>::Three& inTranslation)
    {
        addTranslation(
            inTranslation.x,
            inTranslation.y,
            inTranslation.z
        );
    }

    void Camera::addTranslation(float inX, float inY, float inZ)
    {
        Vec<float>::Three translation = m_transform.translation;
        translation += inX * m_forward;
        translation += inY * m_right;
        translation += inZ * m_up;

        updateTranslation(translation);
    }

    const Vec<float>::Three& Camera::getRotation()
    {
        return m_transform.rotation;
    }

    void Camera::setRotation(const Vec<float>::Three& inRotation)
    {
        updateRotation(inRotation);
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
        Vec<float>::Three rotation = m_transform.rotation;
        rotation.x += inRoll;
        rotation.y += inYaw;
        rotation.z += inPitch;

        updateRotation(rotation);
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

    void Camera::updateTranslation(const Vec<float>::Three& inTranslation)
    {
        bool didChange = std::abs(inTranslation.x - m_transform.translation.x) < FLT_EPSILON ||
                         std::abs(inTranslation.y - m_transform.translation.y) < FLT_EPSILON ||
                         std::abs(inTranslation.z - m_transform.translation.z) < FLT_EPSILON;

        if (!didChange)
        {
            return;
        }

        m_transform.translation = inTranslation;

        updateView();
    }

    void Camera::updateRotation(const Vec<float>::Three& inRotation)
    {
        bool didChange = inRotation.x != m_transform.rotation.x ||
                         inRotation.y != m_transform.rotation.y ||
                         inRotation.z != m_transform.rotation.z;

        if (!didChange)
        {
            return;
        }

        m_transform.rotation = inRotation;

        updateOrientation();
    }

    void Camera::updateOrientation()
    {
        m_orientation = glm::angleAxis(
            glm::radians(m_transform.rotation.x),
            FORWARD_DIRECTION
        );
        m_orientation *= glm::angleAxis(
            glm::radians(m_transform.rotation.y),
            UP_DIRECTION
        );
        m_orientation *= glm::angleAxis(
            glm::radians(m_transform.rotation.z),
            RIGHT_DIRECTION
        );

        m_orientation = glm::normalize(m_orientation);

        m_forward       = glm::rotate(m_orientation, FORWARD_DIRECTION);
        m_UBO.forward.x = m_forward.x;
        m_UBO.forward.y = m_forward.y;
        m_UBO.forward.z = m_forward.z;

        m_right       = glm::rotate(m_orientation, RIGHT_DIRECTION);
        m_UBO.right.x = m_right.x;
        m_UBO.right.y = m_right.y;
        m_UBO.right.z = m_right.z;

        m_up       = glm::rotate(m_orientation, UP_DIRECTION);
        m_UBO.up.x = m_up.x;
        m_UBO.up.y = m_up.y;
        m_UBO.up.z = m_up.z;

        updateView();
    }

    void Camera::updateProjection()
    {
        m_UBO.projection = glm::perspective(
            glm::radians(m_fov),
            m_aspectRatio,
            m_nearClip,
            m_farClip
        );
        // Normalize OpenGL's to Vulkan's coordinate system
        m_UBO.projection[1][1] *= -1;

        updateViewProjection();
    }

    void Camera::updateView()
    {
        m_UBO.view = glm::lookAt(
            m_transform.translation,
            m_transform.translation + m_forward,
            m_up
        );

        updateViewProjection();
    }

    void Camera::updateViewProjection()
    {
        m_UBO.viewProjection = m_UBO.projection * m_UBO.view * Mat<float>::Four(1.0f);
    }
}