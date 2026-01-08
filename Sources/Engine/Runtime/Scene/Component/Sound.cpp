#include "Chicane/Runtime/Scene/Component/Sound.hpp"

namespace Chicane
{
    CSound::CSound()
        : Super(),
        m_sound(),
        m_manager(Box::getSoundManager())
    {}

    void CSound::load(const String& inSound)
    {
        if (inSound.isEmpty())
        {
            return;
        }

        m_manager->activate(inSound);

        m_sound = Screech::Sound(m_manager->getData(inSound));
    }

    void CSound::play()
    {
        m_sound.play();
    }
}