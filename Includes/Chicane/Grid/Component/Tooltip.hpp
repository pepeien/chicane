#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Tooltip : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Tooltip";

            // Attributes
            static constexpr inline const char* ANCHOR_ID_ATTRIBUTE_NAME         = "anchorId";
            static constexpr inline const char* TITLE_ATTRIBUTE_NAME             = "title";
            static constexpr inline const char* IS_OPEN_ATTRIBUTE_NAME           = "isOpen";
            static constexpr inline const char* ON_CLOSE_ATTRIBUTE_NAME          = "onClose";
            static constexpr inline const char* DESCRIPTION_DELAY_ATTRIBUTE_NAME = "contentDelay";

            // Values
            static constexpr inline const float DEFAULT_CONTENT_DELAY_IN_MS = 400.0f;

        public:
            CH_CONSTRUCTOR()
            Tooltip(const XmlNode& inNode);

        public:
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;
            void onRefresh() override;

            void refreshPosition() override;

        public:
            CH_FUNCTION()
            void dismiss();

        private:
            void refreshAttributes();
            void refreshVisibility();
            bool isPinned() const;
            Component* findAnchor() const;
            bool isAnchorHovered(const Component* inAnchor) const;

        public:
            CH_FIELD()
            String anchorId;

            CH_FIELD()
            String title;

            CH_FIELD()
            bool hasTitle;

            CH_FIELD()
            bool hasContent;

            CH_FIELD()
            bool isVisible;

            CH_FIELD()
            String panelState;

            CH_FIELD()
            float contentDelay;
        };
    }
}
