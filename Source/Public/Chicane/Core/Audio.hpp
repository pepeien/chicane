#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Audio
    {
        typedef std::vector<unsigned char> Instance;

        struct Data
        {
        public:
            SDL_AudioSpec    specification = {};
            std::uint32_t    bufferLength  = 0;
            std::uint8_t*    bufferData    = nullptr;
            SDL_AudioStream* stream        = nullptr;
        };
    }
}