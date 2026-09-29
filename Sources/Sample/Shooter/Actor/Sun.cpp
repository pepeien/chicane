#include "Sample/Shooter/Actor/Sun.reflected.hpp"

#include <cmath>

Sun::Sun()
    : Chicane::ALight(),
      m_distance(200.0f),
      m_angle(0.0f)
{}

void Sun::onLoad()
{
    Chicane::ALight::onLoad();

    setAbsoluteTranslation(0.0f, 0.0f, m_distance);
    lookAt(Chicane::Vec3::sZero());

    const float distanceToFocus = std::sqrt(2.0f) * m_distance;
    light->setNearClip(1.0f);
    light->setFarClip(distanceToFocus + m_distance);
    light->setViewport(50.0f, 50.0f);
    light->setProjectionType(Chicane::ViewProjectionType::Orthographic);
    light->setType(Chicane::LightType::Directional);
    light->setCanCastShadows(true);
    light->setColor(Chicane::Vec3(1.0f));
    light->setIntensity(1.0f);
}
