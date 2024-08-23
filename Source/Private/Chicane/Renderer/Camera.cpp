#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
        : m_transform({}),
        m_forward(FORWARD_DIRECTION),
        m_up(UP_DIRECTION),
        m_right(RIGHT_DIRECTION),
        m_viewport(Vec<2, std::uint32_t>(0)),
        m_aspectRatio(0.0f),
        m_fieldOfView(45.0f),
        m_UBO({})
    {
        setClip(0.1f, 1000.0f);
    }

    const Vec<2, std::uint32_t>& Camera::getViewport() const
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

    void Camera::setViewport(const Vec<2, std::uint32_t>& inViewportResolution)
    {
        setViewport(
            inViewportResolution.x,
            inViewportResolution.y
        );
    }

    float Camera::getFieldOfView() const
    {
        return m_fieldOfView;
    }

    void Camera::setFieldOfView(float inFov)
    {
        m_fieldOfView = inFov;

        updateProjection();
    }

    float Camera::getNearClip() const
    {
        return  m_UBO.clip.x;
    }

    void Camera::setNearClip(float inNearClip)
    {
        if (fabs(inNearClip - m_UBO.clip.x) > FLT_EPSILON)
        {
            return;
        }

        m_UBO.clip.x = inNearClip;

        updateProjection();
    }

    float Camera::getFarClip() const
    {
        return m_UBO.clip.y;
    }

    void Camera::setFarClip(float inFarClip)
    {
        if (fabs(inFarClip - m_UBO.clip.y) > FLT_EPSILON)
        {
            return;
        }

        m_UBO.clip.y = inFarClip;

        updateProjection();
    }

    void Camera::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Vec<3, float>& Camera::getTranslation() const
    {
        return m_transform.translation;
    }

    void Camera::setTranslation(const Vec<3, float>& inPosition)
    {
        updateTranslation(inPosition);
    }

    void Camera::addTranslation(const Vec<3, float>& inTranslation)
    {
        addTranslation(
            inTranslation.x,
            inTranslation.y,
            inTranslation.z
        );
    }

    void Camera::addTranslation(float inX, float inY, float inZ)
    {
        Vec<3, float> translation = m_transform.translation;
        translation += m_right * inX;
        translation += m_forward * inY;
        translation += m_up * inZ;

        updateTranslation(translation);
    }

    const Vec<3, float>& Camera::getRotation() const
    {
        return m_transform.rotation;
    }

    void Camera::setRotation(const Vec<3, float>& inRotation)
    {
        updateRotation(inRotation);
    }

    void Camera::addRotation(const Vec<3, float>& inRotation)
    {
        addRotation(
            inRotation.x,
            inRotation.y,
            inRotation.z
        );
    }

    void Camera::addRotation(float inRoll, float inYaw, float inPitch)
    {
        Vec<3, float> rotation = m_transform.rotation;
        rotation.x += inRoll;
        rotation.y += inYaw;
        rotation.z += inPitch;

        updateRotation(rotation);
    }

    const Vec<3, float>& Camera::getForward() const
    {
        return m_forward;
    }

    const Vec<3, float>& Camera::getRight() const
    {
        return m_right;
    }

    const Vec<3, float>& Camera::getUp() const
    {
        return m_up;
    }

    const Camera::UBO& Camera::getUBO() const
    {
        return m_UBO;
    }

    void Camera::updateTranslation(const Vec<3, float>& inTranslation)
    {
        bool isIdentical = std::fabs(m_transform.translation.x - inTranslation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.y - inTranslation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.z - inTranslation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_transform.translation = inTranslation;

        updateView();
    }

    void Camera::updateRotation(const Vec<3, float>& inRotation)
    {
        bool isIdentical = std::fabs(m_transform.rotation.x - inRotation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.y - inRotation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.z - inRotation.z) < FLT_EPSILON;

        if (isIdentical)
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
            glm::radians(m_fieldOfView),
            m_aspectRatio,
            m_UBO.clip.x,
            m_UBO.clip.y
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
        m_UBO.viewProjection = m_UBO.projection * m_UBO.view;
    }
}