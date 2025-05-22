#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            using Raw = std::vector<unsigned char>;

            struct CHICANE_BOX Parsed
            {
            public:
                SDL_AudioSpec    specification = {};
                std::uint32_t    bufferLength  = 0;
                std::uint8_t*    bufferData    = nullptr;
                SDL_AudioStream* stream        = nullptr;
            };
        }
    }
}