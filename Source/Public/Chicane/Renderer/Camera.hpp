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
            Mat<float>::Four view;
            Mat<float>::Four projection;
            Mat<float>::Four viewProjection;

            Vec<float>::Four forward;
            Vec<float>::Four right;
            Vec<float>::Four up;
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
        const Vec<std::uint32_t>::Two& getViewport();
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<std::uint32_t>::Two& inViewportResolution);

        float getFov();
        void setFov(float inFov);

        float getNearClip();
        void setNearClip(float inNearClip);
        float getFarClip();
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        // Positioning
        const Vec<float>::Three& getTranslation();
        void setTranslation(const Vec<float>::Three& inTranslation);
        void addTranslation(float inX, float inY, float inZ);
        void addTranslation(const Vec<float>::Three& inTranslation);

        const Vec<float>::Three& getRotation();
        void setRotation(const Vec<float>::Three& inRotation);
        void addRotation(float inRoll, float inYaw, float inPitch);
        void addRotation(const Vec<float>::Three& inRotation);

        // State
        const Vec<float>::Three& getForward();
        const Vec<float>::Three& getRight();
        const Vec<float>::Three& getUp();

        // Render
        const UBO& getUBO();

    protected:
        // Updates
        void onSettingsUpdate();

        void onRotationUpdate(const Vec<float>::Three& inRotation);
        void updateRotation();

        void onTranslationUpdate(const Vec<float>::Three& inTranslation);

        void onTransformUpdate();

    protected:
        // State
        Transform m_transform;

        Quat<float>::Default m_orientation;

        Vec<float>::Three m_forward;
        Vec<float>::Three m_up;
        Vec<float>::Three m_right;

        // Settings
        Vec<std::uint32_t>::Two m_viewport;
        float m_aspectRatio;

        float m_fov;
        float m_nearClip;
        float m_farClip;

        // Render
        UBO m_UBO;
    };
}