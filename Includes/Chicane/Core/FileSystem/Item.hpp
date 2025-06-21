#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Item
        {
        public:
            enum class Type : std::uint8_t
            {
                Undefined,
                Folder,
                File
            };

        public:
            Type          type       = Type::Undefined;
            String        name       = "";
            String        path       = "";

            // File only
            String        extension  = "";

            // Folder only
            std::uint32_t childCount = 0;
        };
    }
}