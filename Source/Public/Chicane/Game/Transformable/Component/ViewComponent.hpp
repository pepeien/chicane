#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"
#include "Chicane/Renderer/View.hpp"

namespace Chicane
{
    class ViewComponent : public Component
    {
    public:
        ViewComponent();
        virtual ~ViewComponent() = default;

    public:
        bool canSee(const Transformable* inSubject) const;

        const Vec<2, std::uint32_t>& getViewport() const;
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<2, std::uint32_t>& inResolution);

        float getAspectRatio() const;

        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        float getNearClip() const;
        void setNearClip(float inNearClip);
        float getFarClip() const;
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        const View::Data& getData() const;

    protected:
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        View::Settings      m_settings;
        View::SphereFrustum m_frustum;
        View::Data          m_data;
    }; 
}