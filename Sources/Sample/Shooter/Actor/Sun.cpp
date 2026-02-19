#include "Actor/Sun.hpp"

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

    m_light->setFarClip(m_distance);
    m_light->setViewport(m_distance, m_distance);
    m_light->setProjectionType(Chicane::ViewProjectionType::Orthographic);
}