#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Audio
        {
            typedef std::vector<unsigned char> RawData;

            struct CompiledData
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