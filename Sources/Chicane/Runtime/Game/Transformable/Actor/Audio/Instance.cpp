#include "Chicane/Runtime/Game/Transformable/Actor/Audio/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    AAudio::AAudio()
        : Super(),
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