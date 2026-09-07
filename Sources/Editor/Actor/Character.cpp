#include "Editor/Actor/Character.reflected.hpp"

#include <Chicane/Runtime/Scene.hpp>

namespace Editor
{
    Character::Character()
        : Chicane::ACharacter(),
          m_navigation(),
          m_camera(nullptr)
    {}

    void Character::onLoad()
    {
        Chicane::ACharacter::onLoad();

        setAbsoluteTranslation(10.0f, -10.0f, 10.0f);

        m_camera = getScene()->createComponent<Chicane::CCamera>();
        m_camera->attachTo(this);
        m_camera->activate();

        m_navigation.attach(this, m_camera);
    }

    void Character::onControlAttachment()
    {
        m_navigation.bind(m_controller);
    }
}
