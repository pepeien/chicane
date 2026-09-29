#pragma once

#include <cstdint>

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
            static constexpr inline const char* IS_RESIZABLE_ATTRIBUTE_NAME = "isResizable";
            static constexpr inline const char* ON_CLOSE_ATTRIBUTE_NAME     = "onClose";

            // Value
            static constexpr inline const char* BAR_CLASS_NAME = "window__bar";

        public:
            static Window* sFindFrom(Component* inComponent);

        public:
            CH_CONSTRUCTOR()
            Window(const XmlNode& inNode);

        public:
            bool onEvent(const WindowEvent& inEvent) override;
            void tick(float inDeltaTime) override;

        protected:
            void refreshSize() override;
            void refreshPosition() override;

        public:
            CH_FUNCTION()
            void dismiss();

        public:
            bool isGrabbable() const;
            void setGrabbable(bool inValue);

            bool isResizable() const;
            void setResizable(bool inValue);

            bool hasAssignedHandle() const;
            bool isAssignedHandle(const Component* inComponent) const;
            Component* findAssignedHandle() const;

        private:
            void refreshOpenState();
            void refreshTitleVisibility();

            bool canMoveFrom(Component* inHit) const;
            void beginMove(const Vec2& inLocation);
            void updateMove(const Vec2& inLocation);
            void endMove();
            void shift(const Vec2& inDelta);

            std::uint8_t hitResize(const Vec2& inLocation) const;
            void beginResize(std::uint8_t inEdge, const Vec2& inLocation);
            void updateResize(const Vec2& inLocation);
            void endResize();
            void applyExtent(float inWidth, float inHeight);
            void applyResizeCursor(std::uint8_t inEdge);
            void refreshResizeCursor(const Vec2& inLocation);
            void clearCursor();
            float resizeGrip() const;

        public:
            CH_FIELD()
            bool bIsVisible;

            CH_FIELD()
            bool hasTitle;

            CH_FIELD()
            String title;

        private:
            String       m_handleId;
            bool         m_bIsGrabbable;
            bool         m_bIsResizable;
            bool         m_bIsMoving;
            bool         m_bHasExtent;
            Vec2         m_move;
            Vec2         m_moveCursor;
            Vec2         m_extent;
            std::uint8_t m_resizeEdge;
            Vec2         m_resizeCursor;
        };
    }
}
