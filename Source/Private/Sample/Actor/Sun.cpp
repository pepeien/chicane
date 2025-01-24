#include "Actor/Sun.hpp"

Sun::Sun()
    : Chicane::Light(),
    m_radius(800.0f)
{
    setWorldTranslation(0.0f, 0.0f, m_radius);
    setWorldRotation(-90.0f, 0.0f, 0.0f);
}