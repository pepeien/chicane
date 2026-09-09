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

    float CLight::getInnerAngle() const
    {
        return m_innerAngle;
    }

    void CLight::setInnerAngle(float inAngle)
    {
        m_innerAngle = std::clamp(inAngle, 0.0f, 89.9f);
    }

    float CLight::getOuterAngle() const
    {
        return m_outerAngle;
    }

    void CLight::setOuterAngle(float inAngle)
    {
        m_outerAngle = std::clamp(inAngle, 0.0f, 89.9f);
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
        light.innerAngle      = std::min(m_innerAngle, m_outerAngle);
        light.outerAngle      = m_outerAngle;
        light.translation     = Vec3(m_data.translation.x, m_data.translation.y, m_data.translation.z);

        const Vec3 forward = Vec3(m_data.forward.x, m_data.forward.y, m_data.forward.z);
        light.direction    = forward.dot(forward) < 1e-8f ? Vec3::Forward() : forward.normalize();

        return light;
    }
}
