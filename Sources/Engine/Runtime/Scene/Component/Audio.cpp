#include "Chicane/Runtime/Scene/Component/Audio.hpp"

namespace Chicane
{
    CAudio::CAudio()
        : Super(),
        m_audio(""),
        m_manager(Box::getSoundManager())
    {}

    void CAudio::load(const String& inAudio)
    {
        if (inAudio.isEmpty())
        {
            return;
        }

        if (m_audio.equals(inAudio))
        {
            return;
        }

        if (!m_audio.isEmpty())
        {
            m_manager->deactivate(m_audio);
        }

        m_audio = inAudio;

        m_manager->activate(m_audio);
    }

    void CAudio::play() const
    {
        if (m_audio.isEmpty())
        {
            return;
        }

        if (!m_manager->isActive(m_audio))
        {
            return;
        }

        m_manager->getData(m_audio).play();
    }
}