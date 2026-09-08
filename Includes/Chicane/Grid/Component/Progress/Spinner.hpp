#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Type = (Manual), Alias = (Progress::Spinner))
        class CHICANE_GRID ProgressSpinner : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Progress::Spinner";

            // Attributes
            static constexpr inline const char* SPINNING_ATTRIBUTE_NAME = "isSpinning";

            // Values
            static constexpr inline const char* STATE_SPINNING = "spinning";
            static constexpr inline const char* STATE_IDLE     = "idle";

        public:
            CH_CONSTRUCTOR()
            ProgressSpinner(const pugi::xml_node& inNode);

        protected:
            void onTick(float inDeltaTime) override;

        private:
            void refreshSpinning();
            bool parseSpinning(const String& inValue, bool inFallback) const;

        public:
            CH_FIELD()
            bool isSpinning;

            CH_FIELD()
            String spinState;
        };
    }
}
