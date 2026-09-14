#include "Chicane/Runtime/Scene/Actor/Sky.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ASky::ASky()
        : Actor(),
          sky(),
          intensity(0.35f),
          exposure(1.0f),
          m_asset(nullptr),
          m_environment(nullptr)
    {}

    void ASky::onLoad()
    {
        if (!m_environment)
        {
            for (Component* attachment : getAttachments())
            {
                if (CLight* light = dynamic_cast<CLight*>(attachment))
                {
                    m_environment = light;

                    break;
                }
            }
        }

        if (!m_environment)
        {
            m_environment = getScene()->createComponent<CLight>();
            m_environment->attachTo(this);
            m_environment->setType(LightType::Environment);
            m_environment->setColor(Vec3(1.0f));
            m_environment->setCanCastShadows(false);
            m_environment->activate();
        }

        m_environment->setIntensity(intensity);
    }

    void ASky::onPropertyEdited(const String& inName)
    {
        if (inName.equals("sky"))
        {
            if (sky.isEmpty() || !FileSystem::exists(sky))
            {
                m_asset = nullptr;

                return;
            }

            setSky(Box::load<Box::Sky>(sky));

            return;
        }

        if (inName.equals("intensity"))
        {
            setIntensity(intensity);

            return;
        }

        if (inName.equals("exposure"))
        {
            setExposure(exposure);
        }
    }

    const Box::Sky* ASky::getSky() const
    {
        return m_asset;
    }

    void ASky::setSky(const Box::Sky* inSky)
    {
        m_asset = inSky;
        sky     = inSky ? inSky->getFilepath() : FileSystem::Path();
    }

    CLight* ASky::getEnvironmentLight() const
    {
        return m_environment;
    }

    float ASky::getIntensity() const
    {
        return intensity;
    }

    void ASky::setIntensity(float inValue)
    {
        intensity = std::max(inValue, 0.0f);

        if (m_environment)
        {
            m_environment->setIntensity(intensity);
        }
    }

    float ASky::getExposure() const
    {
        return exposure;
    }

    void ASky::setExposure(float inValue)
    {
        exposure = std::max(inValue, 0.0f);
    }
}
