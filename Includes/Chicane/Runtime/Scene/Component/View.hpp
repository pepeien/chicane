#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/View/Frustum.hpp"
#include "Chicane/Core/View/Settings.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME CView : public Component, public ViewSettings
    {
    public:
        CView();

    protected:
        void onTransform() override;
        void onAttachment(Object* inParent) override;
        void onPropertyEdited(const String& inName) override;

    public:
        inline virtual void onResize(const Vec<2, std::uint32_t>& inValue) { return; }

    public:
        // Aspect Ratio
        CH_FUNCTION()
        float getAspectRatio() const;

        // F.O.V
        CH_FUNCTION()
        float getFieldOfView() const;

        CH_FUNCTION()
        void setFieldOfView(float inFov);

        // Clipping
        CH_FUNCTION()
        float getNearClip() const;

        CH_FUNCTION()
        void setNearClip(float inNearClip);

        CH_FUNCTION()
        float getFarClip() const;

        CH_FUNCTION()
        void setFarClip(float inFarClip);

        CH_FUNCTION()
        void setClip(float inNearClip, float inFarClip);

        // Focus
        CH_FUNCTION()
        const Vec3& getFocusPoint() const;

        CH_FUNCTION()
        void setFocusPoint(const Vec3& inPoint);

        // Target
        CH_FUNCTION()
        const String& getTarget() const;

        CH_FUNCTION()
        void setTarget(const String& inValue);

        // Type
        CH_FUNCTION()
        const ViewProjectionType getProjectionType() const;

        CH_FUNCTION()
        void setProjectionType(ViewProjectionType inType);

    public:
        // Frustum
        bool canSee(const Transformable* inSubject) const;
        const ViewFrustum& getFrustum() const;

        // Viewport
        const Vec<2, std::uint32_t>& getViewport() const;
        void setViewport(const Vec<2, std::uint32_t>& inViewport);
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);

        // Data
        const View& getData() const;

    protected:
        void updateProjection();
        void updateView();

    protected:
        ViewFrustum m_frustum;
        View        m_data;
        Vec3        m_focusPoint;
        String      m_target;
    };
}