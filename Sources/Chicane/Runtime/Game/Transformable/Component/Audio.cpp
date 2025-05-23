#include "Chicane/Runtime/Game/Transformable/Component/Audio.hpp"

namespace Chicane
{
    CAudio::CAudio()
        : Super(),
        m_audio("")
    {}

    void CAudio::load(const std::string& inAudio)
    {
        if (inAudio.empty())
        {
            return;
        }

        m_audio = inAudio;

        Box::load(m_audio);
    }

    void CAudio::play() const
    {
        if (m_audio.empty())
        {
            return;
        }

        Box::Audio::Manager* manager = Box::getAudioManager();

        if (!manager->isActive(m_audio))
        {
            return;
        }

        const Box::Audio::Parsed& audio = manager->getData(m_audio);

        SDL_ClearAudioStream(audio.stream);
        SDL_ResumeAudioStreamDevice(audio.stream);
        SDL_PutAudioStreamData(
            audio.stream,
            audio.bufferData,
            audio.bufferLength
        );    
    }
}