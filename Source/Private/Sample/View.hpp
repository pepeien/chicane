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
    std::uint64_t getFPS(const Chicane::Grid::ComponentEvent& inEvent);
    float getFrametime(const Chicane::Grid::ComponentEvent& inEvent);

private:
    void loadAudio(const std::string& inId, const std::string& inFilepath);
    void playAudio(const std::string& inId);

private:
    std::any m_didPlayerWin;

    std::unordered_map<std::string, Audio> m_audios;
};