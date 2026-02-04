#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    CView::CView()
        : Component(),
          m_settings({}),
          m_frustum({}),
          m_data({}),
          m_focusPoint(Vec3::Zero)
    {}

    void CView::onTransform()
    {
        m_data.forward = Vec4(getForward(), 0.0f);
        m_data.right   = Vec4(getRight(), 0.0f);
        m_data.up      = Vec4(getUp(), 0.0f);

        m_data.translation = Vec4(getTranslation(), 0.0f);

        updateView();

        setFocusPoint(getTranslation() + getForward());
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
        setViewport(Vec<2, std::uint32_t>(inWidth, inHeight));
    }

    void CView::setViewport(const Vec<2, std::uint32_t>& inViewport)
    {
        m_settings.viewport    = inViewport;
        m_settings.aspectRatio = 0;

        if (inViewport.x > 0 && inViewport.y > 0)
        {
            m_settings.aspectRatio = (float)m_settings.viewport.x / m_settings.viewport.y;
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

        m_settings.farClip = inFarClip;

        updateProjection();
    }

    void CView::setClip(float inNearClip, float inFarClip)
    {
        setNearClip(inNearClip);
        setFarClip(inFarClip);
    }

    const Vec3& CView::getFocusPoint() const
    {
        return m_focusPoint;
    }

    void CView::setFocusPoint(const Vec3& inPoint)
    {
        m_focusPoint = inPoint;

        updateView();
    }

    const ViewProjectionType CView::getProjectionType() const
    {
        return m_settings.projection;
    }

    void CView::setProjectionType(ViewProjectionType inType)
    {
        m_settings.projection = inType;

        updateProjection();
    }

    const View& CView::getData() const
    {
        return m_data;
    }

    void CView::updateProjection()
    {
        m_data.clip.x = getNearClip();
        m_data.clip.y = getFarClip();

        switch (m_settings.projection)
        {
        case ViewProjectionType::Orthographic:
            m_data.projection = glm::ortho(
                -static_cast<float>(m_settings.viewport.x),
                static_cast<float>(m_settings.viewport.x),
                -static_cast<float>(m_settings.viewport.y),
                static_cast<float>(m_settings.viewport.y),
                m_data.clip.x,
                m_data.clip.y
            );

            break;

        case ViewProjectionType::Perspective:
            m_data.projection =
                glm::perspective(glm::radians(getFieldOfView()), m_settings.aspectRatio, m_data.clip.x, m_data.clip.y);

            break;

        default:
            break;
        }

        m_frustum.update(this, m_settings);
    }

    void CView::updateView()
    {
        m_data.view = glm::lookAt(getTranslation(), m_focusPoint, getUp());

        m_frustum.update(this, m_settings);
    }
}