#include "Actor/Sun.hpp"

Sun::Sun()
    : Chicane::ALight(),
    m_switch(1),
    m_distance(800.0f)
{
    setAbsoluteTranslation(0.0f, 0.0f, m_distance);
    setAbsoluteRotation(-90.0f, 0.0f, 0.0f);
}