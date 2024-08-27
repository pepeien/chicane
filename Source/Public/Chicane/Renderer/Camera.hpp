#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Transformable.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    class Camera : public Transformable
    {
    public:
        struct UBO
        {
            Vec<4, float> clip = Vec<4, float>(0.1f, 1000.0f, 0.0f, 0.0f);

            Mat<4, float> projection     = Mat<4, float>();
            Mat<4, float> view           = Mat<4, float>();
            Mat<4, float> viewProjection = Mat<4, float>();

            Vec<4, float> forward = Vec<4, float>(FORWARD_DIRECTION, 0.0f);
            Vec<4, float> right   = Vec<4, float>(RIGHT_DIRECTION, 0.0f);
            Vec<4, float> up      = Vec<4, float>(UP_DIRECTION, 0.0f);
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
        const Vec<2, std::uint32_t>& getViewport() const;
        void setViewport(std::uint32_t inWidth, std::uint32_t inHeight);
        void setViewport(const Vec<2, std::uint32_t>& inViewportResolution);

        float getFieldOfView() const;
        void setFieldOfView(float inFov);

        float getNearClip() const;
        void setNearClip(float inNearClip);
        float getFarClip() const;
        void setFarClip(float inFarClip);
        void setClip(float inNearClip, float inFarClip);

        // Render
        const UBO& getUBO() const;

    protected:
        // Render
        void updateProjection();
        void updateView();
        void updateViewProjection();

    protected:
        // Settings
        Vec<2, std::uint32_t> m_viewport;
        float m_aspectRatio;
        float m_fieldOfView;

        // Render
        UBO m_UBO;
    };
}