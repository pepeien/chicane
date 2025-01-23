#include "Actor/Sun.hpp"

Sun::Sun()
    : Chicane::Light(),
    m_radius(800.0f)
{
    setTranslation(0.0f, 0.0f, m_radius);
    addRotation(getUp() * -90.0f);
    setScale(m_radius * 0.03f);
}