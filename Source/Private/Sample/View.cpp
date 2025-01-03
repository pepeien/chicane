#include "View.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Content/Sample/Views/Home.grid"
    ),
    m_didPlayerWin(std::make_any<std::string>("false"))
{
    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (m_audios.empty())
            {
                loadAudio(
                    "Hit",
                    "Content/Sample/Sounds/AUD_Hit.wav"
                );
                loadAudio(
                    "Victory",
                    "Content/Sample/Sounds/AUD_Victory.wav"
                );
            }

            if (Game::didReachMaxScore())
            {
                playAudio("Victory");

                m_didPlayerWin = std::make_any<std::string>("true");
            }
            else
            {
                playAudio("Hit");
            }
        }
    );

    addVariable(
        "didPlayerWin",
        &m_didPlayerWin
    );

    addFunction(
        "getFPS",
        std::bind(&View::getFPS, this, std::placeholders::_1)
    );
    addFunction(
        "getFrametime",
        std::bind(&View::getFrametime, this, std::placeholders::_1)
    );
}

View::~View()
{
    for (auto& [id, audio] : m_audios)
    {
        SDL_free(audio.bufferData);
    }
}

std::uint64_t View::getFPS(const Chicane::Grid::Component::Event& inEvent)
{
    return Chicane::getTelemetry().frame.rate;
}

float View::getFrametime(const Chicane::Grid::Component::Event& inEvent)
{
    return Chicane::getTelemetry().frame.time;
}

void View::loadAudio(const std::string& inId, const std::string& inFilepath)
{
    if (!Chicane::FileSystem::exists(inFilepath))
    {
        throw std::runtime_error("The audio file [" + inFilepath + "] doesn't exist");
    }

    Audio audio {};
    bool wasLoaded = SDL_LoadWAV(
        inFilepath.c_str(),
        &audio.specification,
        &audio.bufferData,
        &audio.bufferLength
    );

    if (!wasLoaded)
    {
        throw std::runtime_error(SDL_GetError());
    }

    audio.stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &audio.specification,
        nullptr,
        nullptr
    );

    if (!audio.stream)
    {
        throw std::runtime_error(SDL_GetError());
    }

    m_audios.insert(
        std::make_pair(
            inId,
            audio
        )
    );
}

void View::playAudio(const std::string& inId)
{
    if (m_audios.find(inId) == m_audios.end())
    {
        return;
    }

    const Audio& audio = m_audios.at(inId);

    SDL_ClearAudioStream(audio.stream);
    SDL_ResumeAudioStreamDevice(audio.stream);
    SDL_PutAudioStreamData(
        audio.stream,
        audio.bufferData,
        audio.bufferLength
    );
}