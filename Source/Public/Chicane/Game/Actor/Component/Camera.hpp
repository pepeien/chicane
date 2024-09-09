#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable.hpp"
#include "Chicane/Game/Actor/Component.hpp"
#include "Chicane/Renderer/Camera.hpp"
#include "Chicane/Renderer/Camera/Frustum/Sphere.hpp"

namespace Chicane
{
    class CameraComponent : public ActorComponent
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;

    public:
        void onActivation() override;
        void onDeactivation() override;

    public:
        bool isWithinFrustum(const Transformable* inSubject) const;

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

        const Camera::UBO& getUBO() const;

    protected:
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        Camera::Settings      m_settings;
        Camera::SphereFrustum m_frustum;
        Camera::UBO           m_UBO;
    }; 
}