#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Component/ScrollBar/Properties.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Scrollable : public Component
        {
        public:
            Scrollable(const pugi::xml_node& inNode);
            Scrollable(const String& inTag);
            ~Scrollable() override;

        public:
            void tick(float inDelta) override;
            bool onEvent(const WindowEvent& inEvent) override;
            std::vector<Component*> getChildrenFlat() const override;

            const Vec2& getScroll() const;
            Vec2 getScrollMax() const;
            void addScroll(const Vec2& inValue);
            void addScroll(float inX, float inY);
            void setScroll(const Vec2& inValue);
            void setScroll(float inX, float inY);

            bool clipsOverflow() const;
            bool canScroll() const;
            bool canScrollX() const;
            bool canScrollY() const;

            bool hasScrollBar() const;

        protected:
            bool hitScrollBar(const Vec2& inLocation) const;
            void beginScrollBarDrag(const Vec2& inLocation);
            void updateScrollBarDrag(const Vec2& inLocation);
            void endScrollBarDrag();

            Bounds2D getScrollBarXTrack() const;
            Bounds2D getScrollBarXThumb() const;
            Bounds2D getScrollBarYTrack() const;
            Bounds2D getScrollBarYThumb() const;

            bool isDragging() const;
            bool handleWheel(const Vec2& inDelta);

            void clampScroll();
            void refreshScrollBars();
            void ensureScrollBarParts(ScrollBarProperties& inBar);
            void syncScrollBarPart(
                ScrollBar* inPart, const Bounds2D& inBox, const Color::Rgba& inColor, float inDepthBias
            );

        protected:
            Vec2                m_currentPosition;
            ScrollBarProperties m_horizontalBar;
            ScrollBarProperties m_verticalBar;
        };
    }
}
