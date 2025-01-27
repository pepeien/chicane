#include "Chicane/Game/Transformable/Component/Audio.hpp"

#include "Chicane/Core/Loader.hpp"

namespace Chicane
{
    CAudio::CAudio(const std::string& inAudio)
        : Component(),
        m_audio(inAudio)
    {
        Loader::loadAudio(m_audio);

        activate();
    }

    void CAudio::play() const
    {
        if (m_audio.empty())
        {
            return;
        }

        Audio::Manager* manager = Loader::getAudioManager();

        if (!manager->isActive(m_audio))
        {
            return;
        }

        const Audio::Data& audio = manager->getData(m_audio);

        SDL_ClearAudioStream(audio.stream);
        SDL_ResumeAudioStreamDevice(audio.stream);
        SDL_PutAudioStreamData(
            audio.stream,
            audio.bufferData,
            audio.bufferLength
        );    
    }
}