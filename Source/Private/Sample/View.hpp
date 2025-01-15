#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid.hpp"

struct Audio
{
    SDL_AudioSpec    specification;
    std::uint32_t    bufferLength = 0;
    std::uint8_t*    bufferData   = nullptr;
    SDL_AudioStream* stream       = nullptr;
};

class View : public Chicane::Grid::View
{
public:
    View();
    ~View();

private:
    Chicane::Grid::Reference getFPS(const Chicane::Grid::Component::Event& inEvent);
    Chicane::Grid::Reference getFrametime(const Chicane::Grid::Component::Event& inEvent);

private:
    void loadAudio(const std::string& inId, const std::string& inFilepath);
    void playAudio(const std::string& inId);

private:
    bool                                   m_didPlayerWin;
    Chicane::Grid::Reference               m_uiDidPlayerWin;

    std::unordered_map<std::string, Audio> m_audios;
};