#include "Chicane/Runtime/Scene/Component/View.reflected.hpp"

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"

namespace Chicane
{
    CView::CView()
        : Component(),
          ViewSettings(),
          m_frustum({}),
          m_data({}),
          m_focusPoint(Vec3::sZero()),
          m_target("")
    {
        applyDefaultBounds();
    }

    void CView::onAttachment(Object* inParent)
    {
        if (!inParent)
        {
            return;
        }

        inParent->addBounds(getBounds());
    }

    void CView::onPropertyEdited(const String& inName)
    {
        const std::vector<String> parts = inName.split('.');
        const String              name  = parts.empty() ? inName : parts.back();

        if (name.equals("projection"))
        {
            updateProjection();
        }
    }

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

    const ViewFrustum& CView::getFrustum() const
    {
        return m_frustum;
    }

    const Vec<2, std::uint32_t>& CView::getViewport() const
    {
        return viewport;
    }

    void CView::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        setViewport(Vec<2, std::uint32_t>(inWidth, inHeight));
    }

    void CView::setViewport(const Vec<2, std::uint32_t>& inViewport)
    {
        viewport    = inViewport;
        aspectRatio = 0;

        if (inViewport.x > 0 && inViewport.y > 0)
        {
            aspectRatio = (float)viewport.x / viewport.y;
        }

        updateProjection();
    }

    float CView::getAspectRatio() const
    {
        return aspectRatio;
    }

    float CView::getFieldOfView() const
    {
        return fieldOfView;
    }

    void CView::setFieldOfView(float inFov)
    {
        fieldOfView = inFov;

        updateProjection();
    }

    float CView::getNearClip() const
    {
        return nearClip;
    }

    void CView::setNearClip(float inNearClip)
    {
        if (std::fabs(inNearClip - getNearClip()) < FLT_EPSILON)
        {
            return;
        }

        nearClip = inNearClip;

        updateProjection();
    }

    float CView::getFarClip() const
    {
        return farClip;
    }

    void CView::setFarClip(float inFarClip)
    {
        if (std::fabs(inFarClip - getFarClip()) < FLT_EPSILON)
        {
            return;
        }

        farClip = inFarClip;

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
        return projection;
    }

    void CView::setProjectionType(ViewProjectionType inType)
    {
        projection = inType;

        updateProjection();
    }

    const String& CView::getTarget() const
    {
        return m_target;
    }

    void CView::setTarget(const String& inValue)
    {
        m_target = inValue;
    }

    const View& CView::getData() const
    {
        return m_data;
    }

    void CView::updateProjection()
    {
        m_data.clip.x = getNearClip();
        m_data.clip.y = getFarClip();

        switch (projection)
        {
        case ViewProjectionType::Orthographic:
            m_data.projection = Mat4::sOrtho(
                -static_cast<float>(viewport.x),
                static_cast<float>(viewport.x),
                -static_cast<float>(viewport.y),
                static_cast<float>(viewport.y),
                m_data.clip.x,
                m_data.clip.y
            );

            break;

        case ViewProjectionType::Perspective:
            m_data.projection = Mat4::sPerspective(
                getFieldOfView() * Math::DEG_TO_RAD, aspectRatio, m_data.clip.x, m_data.clip.y
            );

            break;

        default:
            break;
        }

        m_frustum.update(this, *this);
    }

    void CView::updateView()
    {
        static const Mat4 viewBasis = QuatFloat::sFromEuler(Vec3(90.0f, 0.0f, 0.0f)).toMatrix();

        m_data.view = (getMatrix() * viewBasis).inverse();

        m_frustum.update(this, *this);
    }
}