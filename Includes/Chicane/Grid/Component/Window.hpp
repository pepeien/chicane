#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
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
        class CHICANE_GRID Window : public Container
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Window";

            // Attributes
            static constexpr inline const char* IS_OPEN_ATTRIBUTE_NAME      = "isOpen";
            static constexpr inline const char* TITLE_ATTRIBUTE_NAME        = "title";
            static constexpr inline const char* HANDLE_ATTRIBUTE_NAME       = "handle";
            static constexpr inline const char* IS_GRABBABLE_ATTRIBUTE_NAME = "isGrabbable";
            static constexpr inline const char* ON_CLOSE_ATTRIBUTE_NAME     = "onClose";

            // Value
            static constexpr inline const char* BAR_CLASS_NAME = "window__bar";

        public:
            static Window* findFrom(Component* inComponent);

        public:
            CH_CONSTRUCTOR()
            Window(const pugi::xml_node& inNode);

        public:
            bool isFocusable() const override;
            bool escapesOverflow() const override;
            bool onEvent(const WindowEvent& inEvent) override;

        protected:
            void onTick(float inDeltaTime) override;
            void refreshPosition() override;

        public:
            CH_FUNCTION()
            void dismiss();

            bool isGrabbable() const;
            void setGrabbable(bool inValue);

            bool hasAssignedHandle() const;
            bool isAssignedHandle(const Component* inComponent) const;
            Component* findAssignedHandle() const;

        private:
            void refreshAttributes();

            bool parseFlag(const String& inValue, bool inFallback) const;

            bool canMoveFrom(Component* inHit) const;
            void beginMove(const Vec2& inLocation);
            void updateMove(const Vec2& inLocation);
            void endMove();

        public:
            CH_FIELD()
            bool bIsVisible;

            CH_FIELD()
            bool hasTitle;

            CH_FIELD()
            String title;

        private:
            String m_handleId;
            bool   m_bIsGrabbable;
            bool   m_bIsMoving;
            Vec2   m_move;
            Vec2   m_moveCursor;
        };
    }
}
