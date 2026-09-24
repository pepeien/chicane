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

        setAbsoluteTranslation(7.8f, -12.1f, 12.5f);

        m_camera = getScene()->createComponent<Chicane::CCamera>();
        m_camera->attachTo(this);
        m_camera->setIsTransient(true);
        m_camera->activate();

        m_navigation.attach(this, m_camera);
    }

    void Character::frame(const Chicane::Vec3& inPosition, const Chicane::Vec3& inPivot)
    {
        m_navigation.frame(inPosition, inPivot);
    }

    void Character::onControlAttachment()
    {
        m_navigation.bind(m_controller);
    }
}
