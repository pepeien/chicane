#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Component.hpp"
#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    class CameraComponent : public Component
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;

    public:
        void onActivation() override;
        void onDeactivation() override;

    public:
        bool canSee(const Transformable* inSubject) const;

        const Vec<2, std::uint32_t>& getViewport() const;
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<2, std::uint32_t>& inViewportResolution);

        float getAspectRatio() const;

        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        float getNearClip() const;
        void setNearClip(float inNearClip);
        float getFarClip() const;
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        const Camera::Data& getData() const;

    protected:
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        Camera::Settings      m_settings;
        Camera::SphereFrustum m_frustum;
        Camera::Data          m_data;
    }; 
}