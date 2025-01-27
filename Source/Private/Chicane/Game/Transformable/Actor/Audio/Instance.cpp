#include "Chicane/Game/Transformable/Actor/Audio/Instance.hpp"

namespace Chicane
{
    AAudio::AAudio(const std::string& inAudio)
        : Actor(),
        m_audio(new CAudio(inAudio))
    {
        m_audio->attachTo(this);
    }

    void AAudio::play() const
    {
        if (!m_audio)
        {
            return;
        }

        m_audio->play();
    }
}