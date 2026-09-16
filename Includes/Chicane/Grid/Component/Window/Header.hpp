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
        CH_TYPE(Type = (Manual), Alias = (Window::Header))
        class CHICANE_GRID WindowHeader : public Container
        {
        public:
            static constexpr inline const char* TAG_ID = "Window::Header";

            static constexpr inline const char* LABEL_ATTRIBUTE_NAME = "label";

        public:
            CH_CONSTRUCTOR()
            WindowHeader(const XmlNode& inNode);

        protected:
            void onTick(float inDeltaTime) override;

        public:
            CH_FUNCTION()
            void onClose();

        private:
            void refreshLabel();

        public:
            CH_FIELD()
            String label;
        };
    }
}
