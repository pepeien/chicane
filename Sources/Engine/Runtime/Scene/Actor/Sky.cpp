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
          m_environmentIntensity(0.35f)
    {}

    void ASky::onLoad()
    {
        m_environment = getScene()->createComponent<CLight>();
        m_environment->attachTo(this);
        m_environment->setType(LightType::Environment);
        m_environment->setColor(Vec3(1.0f));
        m_environment->setIntensity(m_environmentIntensity);
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

    float ASky::getEnvironmentIntensity() const
    {
        return m_environmentIntensity;
    }

    void ASky::setEnvironmentIntensity(float inIntensity)
    {
        m_environmentIntensity = std::max(inIntensity, 0.0f);

        if (m_environment)
        {
            m_environment->setIntensity(m_environmentIntensity);
        }
    }
}
