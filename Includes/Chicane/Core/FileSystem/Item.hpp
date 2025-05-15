#pragma once

#include "Chicane/Core/Essential.hpp"

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
            std::string   name       = "";
            std::string   path       = "";

            // File only
            std::string   extension  = "";

            // Folder only
            std::uint32_t childCount = 0;
        };
    }
}