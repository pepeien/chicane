#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box/Instance.hpp"

namespace Chicane
{
    namespace Box
    {
        void parseHeader(const std::string& inRawData, Instance& outInstance);
        Instance readHeader(const std::string& inFilePath);

        void parseData(const std::string& inRawData, Instance& outInstance);
        Instance read(const std::string& inFilePath);
    }
}