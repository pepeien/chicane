#include "Chicane/Runtime/Scene/Actor/Sky.reflected.hpp"

#include <algorithm>

#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor(),
          m_asset(nullptr),
          m_environment(nullptr),
          m_intensity(0.35f),
          m_exposure(1.0f)
    {}

    void ASky::onLoad()
    {
        m_environment = getScene()->createComponent<CLight>();
        m_environment->attachTo(this);
        m_environment->setType(LightType::Environment);
        m_environment->setColor(Vec3(1.0f));
        m_environment->setIntensity(m_intensity);
        m_environment->setCanCastShadows(false);
        m_environment->activate();
    }

    const Box::Sky* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky* inSky)
    {
        m_asset = inSky;
    }

    CLight* ASky::getEnvironmentLight() const
    {
        return m_environment;
    }

    float ASky::getIntensity() const
    {
        return m_intensity;
    }

    void ASky::setIntensity(float inValue)
    {
        m_intensity = std::max(inValue, 0.0f);

        if (m_environment)
        {
            m_environment->setIntensity(m_intensity);
        }
    }

    float ASky::getExposure() const
    {
        return m_exposure;
    }

    void ASky::setExposure(float inValue)
    {
        m_exposure = std::max(inValue, 0.0f);
    }
}
