#include "Chicane/Game/Actor/Component/Camera.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Renderer/Camera/Frustum/Sphere.hpp"

namespace Chicane
{
    CameraComponent::CameraComponent()
        : ActorComponent(),
        m_settings({}),
        m_frustum({}),
        m_UBO({})
    {
        watchTransform(
            [this](const Transform& inTransform)
            {
                m_UBO.forward.x = m_direction.forward.x;
                m_UBO.forward.y = m_direction.forward.y;
                m_UBO.forward.z = m_direction.forward.z;

                m_UBO.right.x = m_direction.left.x;
                m_UBO.right.y = m_direction.left.y;
                m_UBO.right.z = m_direction.left.z;

                m_UBO.up.x = m_direction.up.x;
                m_UBO.up.y = m_direction.up.y;
                m_UBO.up.z = m_direction.up.z;

                updateView();
            }
        );
    }

    void CameraComponent::onActivation()
    {
        setActiveCamera(this);

        addComponent(this);
    }

    void CameraComponent::onDeactivation()
    {
        if (getActiveCamera() != this)
        {
            return;
        }

        setActiveCamera(nullptr);
    }

    bool CameraComponent::canSee(const Transformable* inSubject) const
    {
        return m_frustum.contains(inSubject);
    }

    const Vec<2, std::uint32_t>& CameraComponent::getViewport() const
    {
        return m_settings.viewport;
    }

    void CameraComponent::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        m_settings.viewport.x  = inWidth;
        m_settings.viewport.y  = inHeight;
        m_settings.aspectRatio = 0;

        if (inWidth > 0 && inHeight > 0)
        {
            m_settings.aspectRatio = (float) m_settings.viewport.x / m_settings.viewport.y;
        }

        updateProjection();
    }

    void CameraComponent::setViewport(const Vec<2, std::uint32_t>& inViewportResolution)
    {
        setViewport(
            inViewportResolution.x,
            inViewportResolution.y
        );
    }

    float CameraComponent::getAspectRatio() const
    {
        return m_settings.aspectRatio;
    }

    float CameraComponent::getFieldOfView() const
    {
        return m_settings.fieldOfView;
    }

    void CameraComponent::setFieldOfView(float inFov)
    {
        m_settings.fieldOfView = inFov;

        updateProjection();
    }

    float CameraComponent::getNearClip() const
    {
        return m_settings.nearClip;
    }

    void CameraComponent::setNearClip(float inNearClip)
    {
        if (std::fabs(inNearClip - getNearClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inNearClip;

        updateProjection();
    }

    float CameraComponent::getFarClip() const
    {
        return m_settings.farClip;
    }

    void CameraComponent::setFarClip(float inFarClip)
    {
        if (std::fabs(inFarClip - getFarClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inFarClip;

        updateProjection();
    }

    void CameraComponent::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Camera::UBO& CameraComponent::getUBO() const
    {
        return m_UBO;
    }

    void CameraComponent::updateProjection()
    {
        m_UBO.clip.x = getNearClip();
        m_UBO.clip.y = getFarClip();

        m_UBO.projection = glm::perspective(
            glm::radians(getFieldOfView()),
            m_settings.aspectRatio,
            m_UBO.clip.x,
            m_UBO.clip.y
        );
        // Normalize OpenGL's to Vulkan's coordinate system
        m_UBO.projection[1][1] *= -1;

        updateViewProjection();

        m_frustum.use(this);
    }

    void CameraComponent::updateView()
    {
        const Vec<3, float>& translation = getTranslation();

        m_UBO.view = glm::lookAt(
            translation,
            translation + getForward(),
            getUp()
        );

        updateViewProjection();

        m_frustum.use(this);
    }

    void CameraComponent::updateViewProjection()
    {
        m_UBO.viewProjection = m_UBO.projection * m_UBO.view;
    }
}