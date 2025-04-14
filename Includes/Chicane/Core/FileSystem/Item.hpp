#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct Item
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