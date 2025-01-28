#include "Chicane/Game/Transformable/Component/Audio.hpp"

#include "Chicane/Box.hpp"

namespace Chicane
{
    CAudio::CAudio(const std::string& inAudio)
        : Component(),
        m_audio(inAudio)
    {
        Box::loadAudio(m_audio);

        activate();
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

        const Box::Audio::CompiledData& audio = manager->getData(m_audio);

        SDL_ClearAudioStream(audio.stream);
        SDL_ResumeAudioStreamDevice(audio.stream);
        SDL_PutAudioStreamData(
            audio.stream,
            audio.bufferData,
            audio.bufferLength
        );    
    }
}