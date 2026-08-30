#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class Viewport : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Viewport";

            // Attributes
            static constexpr inline const char* TARGET_ATTRIBUTE_NAME = "target";
            static constexpr inline const char* DEFAULT_TARGET        = "Screen";

        public:
            CH_CONSTRUCTOR()
            Viewport(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;
            void onTick(float inDeltaTime) override;

        private:
            void refreshTarget();

        public:
            CH_FIELD()
            String target;
        };
    }
}
