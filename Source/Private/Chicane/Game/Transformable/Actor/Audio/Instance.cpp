#include "Chicane/Game/Transformable/Actor/Audio/Instance.hpp"

#include "Chicane/Application.hpp"

namespace Chicane
{
    AAudio::AAudio(const std::string& inAudio)
        : Actor(),
        m_audio(nullptr)
    {
        m_audio = Application::getLevel()->createComponent<CAudio>();
        m_audio->attachTo(this);
    }

    void AAudio::load(const std::string& inAudio)
    {
        if (inAudio.empty())
        {
            return;
        }

        m_audio->load(inAudio);
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