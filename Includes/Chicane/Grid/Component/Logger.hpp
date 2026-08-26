#pragma once

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Logger : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Logger";

        public:
            CH_CONSTRUCTOR()
            Logger(const pugi::xml_node& inNode);

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FIELD()
            Log::List logs;
        };
    }
}