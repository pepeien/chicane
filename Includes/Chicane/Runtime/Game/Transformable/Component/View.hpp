#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CView : public Component
    {
    public:
        CView();
        virtual ~CView() = default;

    protected:
        virtual void onTransform();

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
        const View::Data& getData() const;

        // Focus
        const Vec3& getFocusPoint() const;
        void setFocusPoint(const Vec3& inPoint);

    protected:
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        View::Settings m_settings;
        View::Frustum  m_frustum;
        View::Data     m_data;
        Vec3           m_focusPoint;
    }; 
}