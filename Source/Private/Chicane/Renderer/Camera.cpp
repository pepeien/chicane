#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    Camera::Camera()
        : m_viewport(Vec<2, std::uint32_t>(0)),
        m_aspectRatio(0.0f),
        m_fieldOfView(45.0f),
        m_UBO({})
    {
        setClip(0.1f, 1000.0f);

        watchTransform(
            [this](const Transform& inTransform)
            {
                m_UBO.forward.x = m_direction.forward.x;
                m_UBO.forward.y = m_direction.forward.y;
                m_UBO.forward.z = m_direction.forward.z;

                m_UBO.right.x = m_direction.right.x;
                m_UBO.right.y = m_direction.right.y;
                m_UBO.right.z = m_direction.right.z;

                m_UBO.up.x = m_direction.up.x;
                m_UBO.up.y = m_direction.up.y;
                m_UBO.up.z = m_direction.up.z;

                updateView();
            }
        );
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

    const Camera::UBO& Camera::getUBO() const
    {
        return m_UBO;
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
            m_transform.translation + getForward(),
            getUp()
        );

        updateViewProjection();
    }

    void Camera::updateViewProjection()
    {
        m_UBO.viewProjection = m_UBO.projection * m_UBO.view;
    }
}