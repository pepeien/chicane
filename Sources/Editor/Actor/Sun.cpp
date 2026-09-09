#include "Editor/Actor/Sun.reflected.hpp"

#include <cmath>

namespace Editor
{
    Sun::Sun()
        : Chicane::ALight(),
          m_distance(200.0f)
    {
        setAbsoluteTranslation(0.0f, 50.0f, m_distance);
        setAbsoluteRotation(-90.0f, 0.0f, 0.0f);
    }

    void Sun::onLoad()
    {
        Chicane::ALight::onLoad();

        const float distanceToFocus = std::sqrt(m_distance * m_distance + 50.0f * 50.0f);
        m_light->setNearClip(1.0f);
        m_light->setFarClip(distanceToFocus + m_distance);
        m_light->setViewport(m_distance, m_distance);
        m_light->setProjectionType(Chicane::ViewProjectionType::Orthographic);
        m_light->setType(Chicane::LightType::Directional);
        m_light->setCanCastShadows(true);
        m_light->setColor(Chicane::Vec3(1.0f));
        m_light->setIntensity(1.0f);
    }
}
