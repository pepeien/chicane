#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/View/Frustum.hpp"
#include "Chicane/Core/View/Settings.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CView : public Component
    {
    public:
        CView();

    public:
        inline virtual void onResize(const Vec<2, std::uint32_t>& inValue) { return; }

    protected:
        void onTransform() override;

    public:
        // Frustum
        bool canSee(const Transformable* inSubject) const;

        // Viewport
        const Vec<2, std::uint32_t>& getViewport() const;
        void setViewport(const Vec<2, std::uint32_t>& inViewport);
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);

        float getAspectRatio() const;

        // F.O.V
        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        // Clipping
        float getNearClip() const;
        void setNearClip(float inNearClip);

        float getFarClip() const;
        void setFarClip(float inFarClip);

        void setClip(float inNearClip, float inFarClip);

        // Data
        const View& getData() const;

        // Focus
        const Vec3& getFocusPoint() const;
        void setFocusPoint(const Vec3& inPoint);

        // Type
        const ViewProjectionType getProjectionType() const;
        void setProjectionType(ViewProjectionType inType);

    protected:
        void updateProjection();
        void updateView();

    protected:
        ViewSettings m_settings;
        ViewFrustum  m_frustum;
        View         m_data;
        Vec3         m_focusPoint;
    };
}