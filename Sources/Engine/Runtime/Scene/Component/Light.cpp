#include "Chicane/Runtime/Scene/Component/Light.reflected.hpp"

#include <algorithm>

namespace Chicane
{
    CLight::CLight()
        : CView()
    {}

    LightType CLight::getType() const
    {
        return m_type;
    }

    void CLight::setType(LightType inType)
    {
        m_type = inType;
    }

    const Vec3& CLight::getColor() const
    {
        return m_color;
    }

    void CLight::setColor(const Vec3& inColor)
    {
        m_color = inColor;
    }

    float CLight::getIntensity() const
    {
        return m_intensity;
    }

    void CLight::setIntensity(float inIntensity)
    {
        m_intensity = std::max(inIntensity, 0.0f);
    }

    float CLight::getRange() const
    {
        return m_range;
    }

    void CLight::setRange(float inRange)
    {
        m_range = std::max(inRange, 0.0f);
    }

    bool CLight::canCastShadows() const
    {
        return m_bCanCastShadows;
    }

    void CLight::setCanCastShadows(bool inValue)
    {
        m_bCanCastShadows = inValue;
    }

    Renderer::Light CLight::getLight() const
    {
        Renderer::Light light = {};
        light.type            = m_type;
        light.castShadows     = m_type == LightType::Environment ? false : m_bCanCastShadows;
        light.color           = m_color;
        light.intensity       = m_intensity;
        light.range           = m_range;
        light.translation     = Vec3(m_data.translation.x, m_data.translation.y, m_data.translation.z);

        Vec3 direction = Vec3(m_data.forward.x, m_data.forward.y, m_data.forward.z).normalize();
        if (direction.dot(direction) < 1e-8f)
        {
            direction = Vec3::Forward();
        }
        light.direction = direction;

        return light;
    }
}
