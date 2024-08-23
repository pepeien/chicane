#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
        : m_transform({}),
        m_forward(FORWARD_DIRECTION),
        m_up(UP_DIRECTION),
        m_right(RIGHT_DIRECTION),
        m_viewport(Math<std::uint32_t, 2>::Vec(0)),
        m_aspectRatio(0.0f),
        m_fieldOfView(45.0f),
        m_UBO({})
    {
        setClip(0.1f, 1000.0f);
    }

    const Math<std::uint32_t, 2>::Vec& Camera::getViewport() const
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

    void Camera::setViewport(const Math<std::uint32_t, 2>::Vec& inViewportResolution)
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

    const Math<float, 3>::Vec& Camera::getTranslation() const
    {
        return m_transform.translation;
    }

    void Camera::setTranslation(const Math<float, 3>::Vec& inPosition)
    {
        updateTranslation(inPosition);
    }

    void Camera::addTranslation(const Math<float, 3>::Vec& inTranslation)
    {
        addTranslation(
            inTranslation.x,
            inTranslation.y,
            inTranslation.z
        );
    }

    void Camera::addTranslation(float inX, float inY, float inZ)
    {
        Math<float, 3>::Vec translation = m_transform.translation;
        translation += m_right * inX;
        translation += m_forward * inY;
        translation += m_up * inZ;

        updateTranslation(translation);
    }

    const Math<float, 3>::Vec& Camera::getRotation() const
    {
        return m_transform.rotation;
    }

    void Camera::setRotation(const Math<float, 3>::Vec& inRotation)
    {
        updateRotation(inRotation);
    }

    void Camera::addRotation(const Math<float, 3>::Vec& inRotation)
    {
        addRotation(
            inRotation.x,
            inRotation.y,
            inRotation.z
        );
    }

    void Camera::addRotation(float inRoll, float inYaw, float inPitch)
    {
        Math<float, 3>::Vec rotation = m_transform.rotation;
        rotation.x += inRoll;
        rotation.y += inYaw;
        rotation.z += inPitch;

        updateRotation(rotation);
    }

    const Math<float, 3>::Vec& Camera::getForward() const
    {
        return m_forward;
    }

    const Math<float, 3>::Vec& Camera::getRight() const
    {
        return m_right;
    }

    const Math<float, 3>::Vec& Camera::getUp() const
    {
        return m_up;
    }

    const Camera::UBO& Camera::getUBO() const
    {
        return m_UBO;
    }

    void Camera::updateTranslation(const Math<float, 3>::Vec& inTranslation)
    {
        if (Math<float, 3>::isIdentical(inTranslation, m_transform.translation))
        {
            return;
        }

        m_transform.translation = inTranslation;

        updateView();
    }

    void Camera::updateRotation(const Math<float, 3>::Vec& inRotation)
    {
        if (Math<float, 3>::isIdentical(inRotation, m_transform.rotation))
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