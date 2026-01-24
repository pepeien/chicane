#include "Chicane/Runtime/Scene/Component/Sound.hpp"

#include "Chicane/Box/Sound/Manager.hpp"

namespace Chicane
{
    CSound::CSound()
        : Component(),
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