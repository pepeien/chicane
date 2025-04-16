#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            typedef std::vector<unsigned char> RawData;

            struct CHICANE CompiledData
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