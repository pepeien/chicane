#include "Sample/Shooter/Actor/Sun.hpp"

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
    lookAt(Chicane::Vec3::Zero());

    const float distanceToFocus = std::sqrt(2.0f) * m_distance;
    m_light->setNearClip(1.0f);
    m_light->setFarClip(distanceToFocus + m_distance);
    m_light->setViewport(50.0f, 50.0f);
    m_light->setProjectionType(Chicane::ViewProjectionType::Orthographic);
}
