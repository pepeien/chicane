#include "Chicane/Game/Transformable/Component/View.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    CView::CView()
        : Component(),
        m_settings({}),
        m_frustum({}),
        m_data({}),
        m_focusPoint(Vec3Zero)
    {}

    void CView::onTransform()
    {
        const Vec<3, float>& forward = getForward();
        m_data.forward.x = forward.x;
        m_data.forward.y = forward.y;
        m_data.forward.z = forward.z;

        const Vec<3, float>& right = getRight();
        m_data.right.x = right.x;
        m_data.right.y = right.y;
        m_data.right.z = right.z;

        const Vec<3, float>& up = getUp();
        m_data.up.x = up.x;
        m_data.up.y = up.y;
        m_data.up.z = up.z;

        const Vec<3, float>& translation = getTranslation();
        m_data.translation.x = translation.x;
        m_data.translation.y = translation.y;
        m_data.translation.z = translation.z;

        updateView();
    }

    bool CView::canSee(const Transformable* inSubject) const
    {
        return m_frustum.contains(inSubject);
    }

    const Vec<2, std::uint32_t>& CView::getViewport() const
    {
        return m_settings.viewport;
    }

    void CView::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        setViewport(inWidth, inHeight);
    }

    void CView::setViewport(const Vec<2, std::uint32_t>& inViewport)
    {
        m_settings.viewport    = inViewport;
        m_settings.aspectRatio = 0;

        if (inViewport.x > 0 && inViewport.y > 0)
        {
            m_settings.aspectRatio = (float) m_settings.viewport.x / m_settings.viewport.y;
        }

        updateProjection();
    }

    float CView::getAspectRatio() const
    {
        return m_settings.aspectRatio;
    }

    float CView::getFieldOfView() const
    {
        return m_settings.fieldOfView;
    }

    void CView::setFieldOfView(float inFov)
    {
        m_settings.fieldOfView = inFov;

        updateProjection();
    }

    float CView::getNearClip() const
    {
        return m_settings.nearClip;
    }

    void CView::setNearClip(float inNearClip)
    {
        if (std::fabs(inNearClip - getNearClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inNearClip;

        updateProjection();
    }

    float CView::getFarClip() const
    {
        return m_settings.farClip;
    }

    void CView::setFarClip(float inFarClip)
    {
        if (std::fabs(inFarClip - getFarClip()) < FLT_EPSILON)
        {
            return;
        }

        m_settings.nearClip = inFarClip;

        updateProjection();
    }

    void CView::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Vec<3, float>& CView::getFocusPoint() const
    {
        return m_focusPoint;
    }

    void CView::setFocusPoint(const Vec<3, float>& inPoint)
    {
        m_focusPoint = inPoint;

        updateView();
    }

    const View::Data& CView::getData() const
    {
        return m_data;
    }

    void CView::updateProjection()
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

    void CView::updateView()
    {
        m_data.view = glm::lookAt(
            getTranslation(),
            m_focusPoint,
            getUp()
        );

        updateViewProjection();
    }

    void CView::updateViewProjection()
    {
        m_data.viewProjection         = m_data.projection * m_data.view;
        m_data.inversedViewProjection = glm::inverse(m_data.viewProjection);

        m_frustum.update(this);
    }
}