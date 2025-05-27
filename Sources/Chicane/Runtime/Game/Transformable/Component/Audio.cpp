#include "Chicane/Runtime/Game/Transformable/Component/Audio.hpp"

namespace Chicane
{
    CAudio::CAudio()
        : Super(),
        m_audio(""),
        m_manager(Box::getSoundManager())
    {}

    void CAudio::load(const std::string& inAudio)
    {
        if (inAudio.empty())
        {
            return;
        }

        if (String::areEquals(m_audio, inAudio))
        {
            return;
        }

        if (!m_audio.empty())
        {
            m_manager->deactivate(m_audio);
        }

        m_audio = inAudio;

        m_manager->activate(m_audio);
    }

    void CAudio::play() const
    {
        if (m_audio.empty())
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