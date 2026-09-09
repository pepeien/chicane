#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Renderer/Light.hpp"
#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component/View.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME CLight : public CView
    {
    public:
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

        bool canCastShadows() const;
        void setCanCastShadows(bool inValue);

        Renderer::Light getLight() const;

    private:
        LightType m_type            = LightType::Directional;
        Vec3      m_color           = Vec3(1.0f);
        float     m_intensity       = 1.0f;
        float     m_range           = 50.0f;
        bool      m_bCanCastShadows = false;
    };
}
