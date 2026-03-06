#include "Sample/Shooter/Actor/Sun.hpp"

Sun::Sun()
    : Chicane::ALight(),
      m_distance(200.0f),
      m_angle(0.0f)
{}

void Sun::onLoad()
{
    Chicane::ALight::onLoad();

    setAbsoluteTranslation(0.0f, 0.0f, m_distance);
    lookAt(Chicane::Vec3::Zero);

    m_light->setFarClip(m_distance);
    m_light->setViewport(100.0f, 100.0f);
    m_light->setProjectionType(Chicane::ViewProjectionType::Orthographic);
}