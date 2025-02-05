#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"
#include "Chicane/Renderer/View.hpp"

namespace Chicane
{
    class CView : public Component
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
        const Vec<3, float>& getFocusPoint() const;
        void setFocusPoint(const Vec<3, float>& inPoint);

    protected:
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        View::Settings m_settings;
        View::Frustum  m_frustum;
        View::Data     m_data;
        Vec<3, float>  m_focusPoint;
    }; 
}