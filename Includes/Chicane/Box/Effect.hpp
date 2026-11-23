// .bvfx
// <Asset version="1" id="Fire">
//     <System name="Flame" duration="0.2">
//         <Spawn shape="Cone" burst="8" lifetime="0.1" speed="4" size="0.1" color="1,0.4,0.05,1" blend="Additive"/>
//         <Renderer::Sprite blend="Additive"/>
//     </System>
// </Asset>

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
