#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        class CHICANE_BOX Effect : public Asset
        {
        public:
            // File
            static constexpr inline const char* EXTENSION = ".bvfx";

            // Tag
            static constexpr inline const char* TAG = "Effect";

        public:
            Effect(const FileSystem::Path& inFilepath);
            virtual ~Effect() = default;
        };
    }
}
