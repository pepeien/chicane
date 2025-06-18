#include "Chicane/Runtime/Scene/Actor/Audio.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    AAudio::AAudio()
        : Super(),
        m_audio(nullptr)
    {
        m_audio = Application::getScene()->createComponent<CAudio>();
        m_audio->attachTo(this);
    }

    void AAudio::load(const String& inAudio)
    {
        if (inAudio.isEmpty())
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