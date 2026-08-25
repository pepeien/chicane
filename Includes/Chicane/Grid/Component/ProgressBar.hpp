#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID ProgressBar : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "ProgressBar";

            // Attributes
            static constexpr inline const char* PERCENTAGE_ATTRIBUTE_NAME = "percentage"; // [`0.0`, `100.0`]

        public:
            CH_CONSTRUCTOR()
            ProgressBar(const pugi::xml_node& inNode);

        protected:
            void onTick(float inDeltaTime) override;

        private:
            void refreshPercentage();

        public:
            CH_FIELD()
            float percentage;
        };
    }
}