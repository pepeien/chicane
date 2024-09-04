#pragma once

#include "Chicane/Box/Entry.hpp"
#include "Chicane/Box/Instance.hpp"
#include "Chicane/Box/Parse.hpp"
#include "Chicane/Box/Write.hpp"

namespace Chicane
{
    namespace Box
    {
        constexpr auto HEADER_SIGNATURE     = "BOX";
        constexpr auto ENTRY_SIGNATURE      = "ENTRY";
        constexpr auto ENTRY_DATA_SIGNATURE = "DATA";
        constexpr auto DATA_SEPARATOR       = ";";
        constexpr auto FILE_EXTENSTION      = ".box";
    }
}