#include "Actor/Sun.hpp"

Sun::Sun()
    : Chicane::Light(),
    m_switch(Chicane::Vec<3, float>(1.0f, 0.0f, 1.0f)),
    m_radius(800.0f),
    m_rate(10.0f / m_radius),
    m_mesh(new Chicane::MeshComponent())
{
    setCanTick(true);

    setAbsoluteTranslation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            m_radius
        )
    );

    setAbsoluteRotation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            90.0f
        )
    );

    setAbsoluteScale(Chicane::Vec<3, float>(20.0f));

    m_mesh->attachTo(this);
    m_mesh->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_mesh->activate();
}

void Sun::onTick(float inDeltaTime)
{
    if (m_transform.translation.x < -m_radius || m_transform.translation.x > m_radius)
    {
        m_switch.x *= -1.0f;
    }

    if (m_transform.translation.z < -m_radius || m_transform.translation.z > m_radius)
    {
        m_switch.z *= -1.0f;
    }

    setRelativeTranslation(m_rate * m_switch);

    setRelativeRotation(
        Chicane::Vec<3, float>(
            m_rate * 0.125f,
            0.0f,
            0.0f
        )
    );
}