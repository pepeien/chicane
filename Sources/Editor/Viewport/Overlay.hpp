#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Math/Vec/Vec4.hpp>

namespace Editor
{
    struct ViewportOverlay
    {
    public:
        static inline ViewportOverlay& get()
        {
            static ViewportOverlay overlay;

            return overlay;
        }

    public:
        bool bGridEnabled = true;
        bool bGridAxisX   = true;
        bool bGridAxisY   = true;
        bool bGridAxisZ   = false;

        Chicane::Vec3 gridColor     = Chicane::Vec3(0.3294f, 0.3294f, 0.3294f);
        float         gridScale     = 0.5f;
        float         gridDivisions = 10.0f;

        Chicane::Vec4 geometryColor = Chicane::Vec4(1.0f, 0.75f, 0.2f, 1.0f);

        Chicane::Vec3 outlinerColor = Chicane::Vec3(0.043f, 0.6f, 0.49f);

        Chicane::Vec4 physicsColor = Chicane::Vec4(0.3f, 1.0f, 0.4f, 1.0f);

        Chicane::Vec3 boneColor       = Chicane::Vec3(0.0f, 0.82f, 1.0f);
    };
}
