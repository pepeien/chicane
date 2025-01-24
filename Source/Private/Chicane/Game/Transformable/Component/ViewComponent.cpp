#include "Chicane/Game/Transformable/Component/ViewComponent.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    ViewComponent::ViewComponent()
        : Component(),
        m_settings({}),
        m_frustum({}),
        m_data({}),
        m_focusPoint(Vec3Zero)
    {}

    void ViewComponent::onTransform()
    {
        m_data.forward.x = m_direction.forward.x;
        m_data.forward.y = m_direction.forward.y;
        m_data.forward.z = m_direction.forward.z;

        m_data.right.x = m_direction.right.x;
        m_data.right.y = m_direction.right.y;
        m_data.right.z = m_direction.right.z;

        m_data.up.x = m_direction.up.x;
        m_data.up.y = m_direction.up.y;
        m_data.up.z = m_direction.up.z;

        m_data.translation.x = m_transform.translation.x;
        m_data.translation.y = m_transform.translation.y;
        m_data.translation.z = m_transform.translation.z;

        updateView();
    }

    bool ViewComponent::canSee(const Transformable* inSubject) const
    {
        return m_frustum.contains(inSubject);
    }

    const Vec<2, std::uint32_t>& ViewComponent::getViewport() const
    {
        return m_settings.viewport;
    }

    void ViewComponent::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
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

    void ViewComponent::setViewport(const Vec<2, std::uint32_t>& inResolution)
    {
        setViewport(inResolution.x, inResolution.y);
    }

    float ViewComponent::getAspectRatio() const
    {
        return m_settings.aspectRatio;
    }

    float ViewComponent::getFieldOfView() const
    {
        return m_settings.fieldOfView;
    }

    void ViewComponent::setFieldOfView(float inFov)
    {
        m_settings.fieldOfView = inFov;

        updateProjection();
    }

    float ViewComponent::getNearClip() const
    {
        return m_settings.nearClip;
    }

    void ViewComponent::setNearClip(float inNearClip)
    {
        if (std::fabs(inNearClip - getNearClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inNearClip;

        updateProjection();
    }

    float ViewComponent::getFarClip() const
    {
        return m_settings.farClip;
    }

    void ViewComponent::setFarClip(float inFarClip)
    {
        if (std::fabs(inFarClip - getFarClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inFarClip;

        updateProjection();
    }

    void ViewComponent::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Vec<3, float>& ViewComponent::getFocusPoint() const
    {
        return m_focusPoint;
    }

    void ViewComponent::setFocusPoint(const Vec<3, float>& inPoint)
    {
        m_focusPoint = inPoint;

        updateView();
    }

    const View::Data& ViewComponent::getData() const
    {
        return m_data;
    }

    void ViewComponent::updateProjection()
    {
        m_data.clip.x = getNearClip();
        m_data.clip.y = getFarClip();

        m_data.projection = glm::perspective(
            glm::radians(getFieldOfView()),
            m_settings.aspectRatio,
            m_data.clip.x,
            m_data.clip.y
        );
        // Normalize OpenGL's to Vulkan's coordinate system
        m_data.projection[1][1] *= -1;

        updateViewProjection();
    }

    void ViewComponent::updateView()
    {
        m_data.view = glm::lookAt(
            getTranslation(),
            m_focusPoint,
            getUp()
        );

        updateViewProjection();
    }

    void ViewComponent::updateViewProjection()
    {
        m_data.viewProjection         = m_data.projection * m_data.view;
        m_data.inversedViewProjection = glm::inverse(m_data.viewProjection);

        m_frustum.use(this);
    }
}