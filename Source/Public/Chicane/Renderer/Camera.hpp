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
            Vec<float>::Four clip = Vec<float>::Four(0.1f, 1000.0f, 0.0f, 0.0f);

            Mat<float>::Four projection     = Mat<float>::Four();
            Mat<float>::Four view           = Mat<float>::Four();
            Mat<float>::Four viewProjection = Mat<float>::Four();

            Vec<float>::Four forward = Vec<float>::Four(FORWARD_DIRECTION, 0.0f);
            Vec<float>::Four right   = Vec<float>::Four(RIGHT_DIRECTION, 0.0f);
            Vec<float>::Four up      = Vec<float>::Four(UP_DIRECTION, 0.0f);
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
        const Vec<std::uint32_t>::Two& getViewport() const;
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<std::uint32_t>::Two& inViewportResolution);

        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        float getNearClip() const;
        void setNearClip(float inNearClip);
        float getFarClip() const;
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        // Transform
        const Vec<float>::Three& getTranslation() const;
        void setTranslation(const Vec<float>::Three& inTranslation);
        void addTranslation(const Vec<float>::Three& inTranslation);
        void addTranslation(float inX, float inY, float inZ);

        const Vec<float>::Three& getRotation() const;
        void setRotation(const Vec<float>::Three& inRotation);
        void addRotation(const Vec<float>::Three& inRotation);
        void addRotation(float inRoll, float inYaw, float inPitch);

        // State
        const Vec<float>::Three& getForward() const;
        const Vec<float>::Three& getRight() const;
        const Vec<float>::Three& getUp() const;

        // Render
        const UBO& getUBO() const;

    protected:
        // Transform
        void updateTranslation(const Vec<float>::Three& inTranslation);
        void updateRotation(const Vec<float>::Three& inRotation);
        void updateOrientation();

        // Render
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        // Transform
        Transform m_transform;

        // State
        Quat<float>::Default m_orientation;

        Vec<float>::Three m_forward;
        Vec<float>::Three m_up;
        Vec<float>::Three m_right;

        // Settings
        Vec<std::uint32_t>::Two m_viewport;
        float m_aspectRatio;
        float m_fieldOfView;

        // Render
        UBO m_UBO;
    };
}