#include "Chicane/Runtime/Scene/Actor/Sound.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    ASound::ASound()
        : Super(),
          m_sound(nullptr)
    {
        m_sound = Application::getScene()->createComponent<CSound>();
        m_sound->attachTo(this);
    }

    void ASound::load(const String& inSound)
    {
        if (inSound.isEmpty())
        {
            return;
        }

        m_sound->load(inSound);
    }

    void ASound::play() const
    {
        if (!m_sound)
        {
            return;
        }

        m_sound->play();
    }
}