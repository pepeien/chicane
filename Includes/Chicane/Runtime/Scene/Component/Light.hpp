#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Renderer/Light.hpp"
#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Light")
    class CHICANE_RUNTIME CLight : public CView
    {
    public:
        static constexpr inline const char* TAG_ID = "CLight";

    public:
        CH_CONSTRUCTOR()
        CLight();

    public:
        LightType getType() const;
        void setType(LightType inType);

        const Vec3& getColor() const;
        void setColor(const Vec3& inColor);

        float getIntensity() const;
        void setIntensity(float inIntensity);

        float getRange() const;
        void setRange(float inRange);

        float getInnerAngle() const;
        void setInnerAngle(float inAngle);

        float getOuterAngle() const;
        void setOuterAngle(float inAngle);

        bool canCastShadows() const;
        void setCanCastShadows(bool inValue);

        Renderer::Light getLight() const;

    public:
        CH_FIELD()
        LightType type = LightType::Directional;

        CH_FIELD()
        Vec3 color = Vec3(1.0f);

        CH_FIELD()
        float intensity = 1.0f;

        CH_FIELD()
        float range = 50.0f;

        CH_FIELD()
        float innerAngle = 25.0f;

        CH_FIELD()
        float outerAngle = 40.0f;

        CH_FIELD()
        bool castShadows = false;
    };
}
