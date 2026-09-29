#include "Chicane/Runtime/Scene/Component/Light.reflected.hpp"

#include <algorithm>

namespace Chicane
{
    CLight::CLight()
        : CView()
    {}

    LightType CLight::getType() const
    {
        return type;
    }

    void CLight::setType(LightType inType)
    {
        type = inType;
    }

    const Vec3& CLight::getColor() const
    {
        return color;
    }

    void CLight::setColor(const Vec3& inColor)
    {
        color = inColor;
    }

    float CLight::getIntensity() const
    {
        return intensity;
    }

    void CLight::setIntensity(float inIntensity)
    {
        intensity = std::max(inIntensity, 0.0f);
    }

    float CLight::getRange() const
    {
        return range;
    }

    void CLight::setRange(float inRange)
    {
        range = std::max(inRange, 0.0f);
    }

    float CLight::getInnerAngle() const
    {
        return innerAngle;
    }

    void CLight::setInnerAngle(float inAngle)
    {
        innerAngle = std::clamp(inAngle, 0.0f, 89.9f);
    }

    float CLight::getOuterAngle() const
    {
        return outerAngle;
    }

    void CLight::setOuterAngle(float inAngle)
    {
        outerAngle = std::clamp(inAngle, 0.0f, 89.9f);
    }

    bool CLight::canCastShadows() const
    {
        return castShadows;
    }

    void CLight::setCanCastShadows(bool inValue)
    {
        castShadows = inValue;
    }

    Renderer::Light CLight::getLight() const
    {
        Renderer::Light light = {};
        light.type            = type;
        light.castShadows     = type == LightType::Environment ? false : castShadows;
        light.color           = color;
        light.intensity       = intensity;
        light.range           = range;
        light.innerAngle      = std::min(innerAngle, outerAngle);
        light.outerAngle      = outerAngle;
        light.translation     = Vec3(m_data.translation.x, m_data.translation.y, m_data.translation.z);

        const Vec3 forward = Vec3(m_data.forward.x, m_data.forward.y, m_data.forward.z);
        light.direction    = forward.dot(forward) < 1e-8f ? Vec3::sForward() : forward.normalize();

        return light;
    }
}
