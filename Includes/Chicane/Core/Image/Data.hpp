#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    namespace Image
    {
        typedef unsigned char*             Pixels;
        typedef std::vector<unsigned char> RawData;

        struct CompiledData
        {
        public:
            int    width    = 0;
            int    height   = 0;
            int    channels = 0;
            Pixels pixels   = nullptr;
        };
    }
}