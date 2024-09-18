#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    namespace Camera
    {
        struct UBO
        {
            Vec<4, float> clip           = Vec<4, float>(0.1f, 1000.0f, 0.0f, 0.0f);

            Mat<4, float> projection     = Mat<4, float>();
            Mat<4, float> view           = Mat<4, float>();
            Mat<4, float> viewProjection = Mat<4, float>();

            Vec<4, float> forward        = Vec<4, float>(FORWARD_DIRECTION, 0.0f);
            Vec<4, float> right          = Vec<4, float>(LEFT_DIRECTION,   0.0f);
            Vec<4, float> up             = Vec<4, float>(UP_DIRECTION,      0.0f);
        };

        struct UBOBundle
        {
            size_t           allocationSize;
            void*            writeLocation;
            Buffer::Instance buffer;
            UBO              instance;
        };

        struct Settings
        {
            Vec<2, std::uint32_t> viewport    = Vec<2, std::uint32_t>(0);
            float                 aspectRatio = 0.0f;
            float                 fieldOfView = 45.0f;
            float                 nearClip    = 0.1f;
            float                 farClip     = 5000.0f;
        };
    }
}