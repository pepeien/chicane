#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    class Camera
    {
    public:
        struct UBO
        {
            Math<float, 4>::Vec clip = Math<float, 4>::Vec(0.1f, 1000.0f, 0.0f, 0.0f);

            Math<float, 4>::Mat projection     = Math<float, 4>::Mat();
            Math<float, 4>::Mat view           = Math<float, 4>::Mat();
            Math<float, 4>::Mat viewProjection = Math<float, 4>::Mat();

            Math<float, 4>::Vec forward = Math<float, 4>::Vec(FORWARD_DIRECTION, 0.0f);
            Math<float, 4>::Vec right   = Math<float, 4>::Vec(RIGHT_DIRECTION, 0.0f);
            Math<float, 4>::Vec up      = Math<float, 4>::Vec(UP_DIRECTION, 0.0f);
        };

        struct UBOBundle
        {
            size_t allocationSize;
            void* writeLocation;
            Buffer::Instance buffer;
            UBO instance;
        };

    public:
        Camera();
        virtual ~Camera() = default;

    public:
        virtual void onEvent(const SDL_Event& inEvent) { return; };

    public:
        // Settings
        const Math<std::uint32_t, 2>::Vec& getViewport() const;
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Math<std::uint32_t, 2>::Vec& inViewportResolution);

        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        float getNearClip() const;
        void setNearClip(float inNearClip);
        float getFarClip() const;
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        // Transform
        const Math<float, 3>::Vec& getTranslation() const;
        void setTranslation(const Math<float, 3>::Vec& inTranslation);
        void addTranslation(const Math<float, 3>::Vec& inTranslation);
        void addTranslation(float inX, float inY, float inZ);

        const Math<float, 3>::Vec& getRotation() const;
        void setRotation(const Math<float, 3>::Vec& inRotation);
        void addRotation(const Math<float, 3>::Vec& inRotation);
        void addRotation(float inRoll, float inYaw, float inPitch);

        // State
        const Math<float, 3>::Vec& getForward() const;
        const Math<float, 3>::Vec& getRight() const;
        const Math<float, 3>::Vec& getUp() const;

        // Render
        const UBO& getUBO() const;

    protected:
        // Transform
        void updateTranslation(const Math<float, 3>::Vec& inTranslation);
        void updateRotation(const Math<float, 3>::Vec& inRotation);
        void updateOrientation();

        // Render
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        // Transform
        Transform m_transform;

        // State
        Math<float>::Quat m_orientation;

        Math<float, 3>::Vec m_forward;
        Math<float, 3>::Vec m_up;
        Math<float, 3>::Vec m_right;

        // Settings
        Math<std::uint32_t, 2>::Vec m_viewport;
        float m_aspectRatio;
        float m_fieldOfView;

        // Render
        UBO m_UBO;
    };
}