#include "Chicane/Runtime/Scene/Actor/Sky.reflected.hpp"

#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor(),
          m_asset(nullptr),
          m_environment(nullptr)
    {}

    void ASky::onLoad()
    {
        m_environment = getScene()->createComponent<CLight>();
        m_environment->attachTo(this);
        m_environment->setType(LightType::Environment);
        m_environment->setColor(Vec3(1.0f));
        m_environment->setIntensity(0.35f);
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
}
