#include "Actor/Sun.hpp"

Sun::Sun()
    : Chicane::ALight(),
    m_radius(800.0f)
{
    setAbsoluteTranslation(0.0f, 0.0f, m_radius);
    setAbsoluteRotation(-90.0f, 0.0f, 0.0f);
}