#include "Chicane/Grid/Component/Scrollable.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"
#include "Chicane/Grid/Style/Overflow.hpp"

namespace Chicane
{
    namespace Grid
    {
        const Color::Rgba SCROLL_BAR_TRACK_COLOR(0, 0, 0, 110);
        const Color::Rgba SCROLL_BAR_THUMB_COLOR(210, 210, 210, 210);

        Scrollable::Scrollable(const pugi::xml_node& inNode)
            : Component(inNode),
              m_currentPosition(Vec2::Zero()),
              m_virtualContentSize(Vec2::Zero()),
              m_bHasVirtualContent(false),
              m_horizontalBar({}),
              m_verticalBar({})
        {}

        Scrollable::Scrollable(const String& inTag)
            : Component(inTag),
              m_currentPosition(Vec2::Zero()),
              m_virtualContentSize(Vec2::Zero()),
              m_bHasVirtualContent(false),
              m_horizontalBar({}),
              m_verticalBar({})
        {}

        Scrollable::~Scrollable()
        {
            m_horizontalBar.destroy();
            m_verticalBar.destroy();
        }

        void Scrollable::tick(float inDelta)
        {
            Component::tick(inDelta);

            clampScroll();
            refreshScrollBars();
        }

        bool Scrollable::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (!isDragging())
                {
                    return false;
                }

                endScrollBarDrag();

                return true;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left || !hitScrollBar(event.location))
                {
                    return false;
                }

                beginScrollBarDrag(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!isDragging())
                {
                    return false;
                }

                Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);
                updateScrollBarDrag(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseWheel)
            {
                Input::MouseWheelEvent event = *static_cast<Input::MouseWheelEvent*>(inEvent.data);
                if (!canScroll() || !containsPoint(event.location))
                {
                    return false;
                }

                return handleWheel(event.delta);
            }

            return false;
        }

        bool Scrollable::isDragging() const
        {
            return m_horizontalBar.bIsDragging || m_verticalBar.bIsDragging;
        }

        bool Scrollable::handleWheel(const Vec2& inDelta)
        {
            const float step = std::max(16.0f, m_style.font.size.get() * 3.0f);

            Vec2 delta = Vec2::Zero();

            if (canScrollX())
            {
                delta.x = -inDelta.x * step;
            }

            if (canScrollY())
            {
                delta.y = -inDelta.y * step;
            }

            if (!canScrollY() && canScrollX() && std::fabs(inDelta.y) > std::fabs(inDelta.x))
            {
                delta.x = -inDelta.y * step;
            }

            addScroll(delta);

            return true;
        }

        std::vector<Component*> Scrollable::getChildrenFlat() const
        {
            std::vector<Component*> result = Component::getChildrenFlat();

            m_verticalBar.append(result);
            m_horizontalBar.append(result);

            return result;
        }

        const Vec2& Scrollable::getScroll() const
        {
            return m_currentPosition;
        }

        Vec2 Scrollable::getScrollMax() const
        {
            const Vec2 content = m_bHasVirtualContent ? m_virtualContentSize : getChildrenContentSize();

            return Vec2(
                std::max(0.0f, content.x + m_style.insetHorizontal() - m_size.x),
                std::max(0.0f, content.y + m_style.insetVertical() - m_size.y)
            );
        }

        void Scrollable::setVirtualContentSize(const Vec2& inValue)
        {
            m_virtualContentSize = inValue;
            m_bHasVirtualContent = true;
        }

        void Scrollable::clearVirtualContentSize()
        {
            m_virtualContentSize = Vec2::Zero();
            m_bHasVirtualContent = false;
        }

        void Scrollable::addScroll(const Vec2& inValue)
        {
            addScroll(inValue.x, inValue.y);
        }

        void Scrollable::addScroll(float inX, float inY)
        {
            setScroll(m_currentPosition.x + inX, m_currentPosition.y + inY);
        }

        void Scrollable::setScroll(const Vec2& inValue)
        {
            setScroll(inValue.x, inValue.y);
        }

        void Scrollable::setScroll(float inX, float inY)
        {
            m_currentPosition.x = inX;
            m_currentPosition.y = inY;

            clampScroll();
            refreshScrollBars();
        }

        bool Scrollable::isClippingOverflow() const
        {
            return m_style.isClippingOverflow();
        }

        bool Scrollable::canScroll() const
        {
            return canScrollX() || canScrollY();
        }

        bool Scrollable::canScrollX() const
        {
            const StyleOverflow overflow = m_style.overflowX.get();

            if (overflow == StyleOverflow::Scroll)
            {
                return true;
            }

            if (overflow == StyleOverflow::Auto)
            {
                return getScrollMax().x > 0.0f;
            }

            return false;
        }

        bool Scrollable::canScrollY() const
        {
            const StyleOverflow overflow = m_style.overflowY.get();

            if (overflow == StyleOverflow::Scroll)
            {
                return true;
            }

            if (overflow == StyleOverflow::Auto)
            {
                return getScrollMax().y > 0.0f;
            }

            return false;
        }

        bool Scrollable::hasScrollBar() const
        {
            return m_horizontalBar.bIsVisible || m_verticalBar.bIsVisible;
        }

        bool Scrollable::hitScrollBar(const Vec2& inLocation) const
        {
            if (m_verticalBar.bIsVisible && getScrollBarYTrack().contains(inLocation))
            {
                return true;
            }

            if (m_horizontalBar.bIsVisible && getScrollBarXTrack().contains(inLocation))
            {
                return true;
            }

            return false;
        }

        void Scrollable::beginScrollBarDrag(const Vec2& inLocation)
        {
            if (m_verticalBar.bIsVisible && getScrollBarYTrack().contains(inLocation))
            {
                const Bounds2D thumb     = getScrollBarYThumb();
                const Bounds2D track     = getScrollBarYTrack();
                const float    thumbSize = thumb.bottom - thumb.top;

                if (!thumb.contains(inLocation))
                {
                    const float travel = (track.bottom - track.top) - thumbSize;
                    if (travel > 0.0f && getScrollMax().y > 0.0f)
                    {
                        const float ratio =
                            std::clamp((inLocation.y - track.top - thumbSize * 0.5f) / travel, 0.0f, 1.0f);
                        setScroll(m_currentPosition.x, ratio * getScrollMax().y);
                    }
                }

                m_verticalBar.grab          = inLocation.y - getScrollBarYThumb().top;
                m_verticalBar.bIsDragging   = true;
                m_horizontalBar.bIsDragging = false;

                return;
            }

            if (m_horizontalBar.bIsVisible && getScrollBarXTrack().contains(inLocation))
            {
                const Bounds2D thumb     = getScrollBarXThumb();
                const Bounds2D track     = getScrollBarXTrack();
                const float    thumbSize = thumb.right - thumb.left;

                if (!thumb.contains(inLocation))
                {
                    const float travel = (track.right - track.left) - thumbSize;
                    if (travel > 0.0f && getScrollMax().x > 0.0f)
                    {
                        const float ratio =
                            std::clamp((inLocation.x - track.left - thumbSize * 0.5f) / travel, 0.0f, 1.0f);
                        setScroll(ratio * getScrollMax().x, m_currentPosition.y);
                    }
                }

                m_horizontalBar.grab        = inLocation.x - getScrollBarXThumb().left;
                m_horizontalBar.bIsDragging = true;
                m_verticalBar.bIsDragging   = false;
            }
        }

        void Scrollable::updateScrollBarDrag(const Vec2& inLocation)
        {
            if (m_verticalBar.bIsDragging && m_verticalBar.bIsVisible)
            {
                const Bounds2D track     = getScrollBarYTrack();
                const float    thumbSize = m_verticalBar.thumb.bottom - m_verticalBar.thumb.top;
                const float    travel    = (track.bottom - track.top) - thumbSize;

                if (travel > 0.0f && getScrollMax().y > 0.0f)
                {
                    const float ratio =
                        std::clamp((inLocation.y - track.top - m_verticalBar.grab) / travel, 0.0f, 1.0f);
                    setScroll(m_currentPosition.x, ratio * getScrollMax().y);
                }
            }

            if (m_horizontalBar.bIsDragging && m_horizontalBar.bIsVisible)
            {
                const Bounds2D track     = getScrollBarXTrack();
                const float    thumbSize = m_horizontalBar.thumb.right - m_horizontalBar.thumb.left;
                const float    travel    = (track.right - track.left) - thumbSize;

                if (travel > 0.0f && getScrollMax().x > 0.0f)
                {
                    const float ratio =
                        std::clamp((inLocation.x - track.left - m_horizontalBar.grab) / travel, 0.0f, 1.0f);
                    setScroll(ratio * getScrollMax().x, m_currentPosition.y);
                }
            }
        }

        void Scrollable::endScrollBarDrag()
        {
            m_horizontalBar.bIsDragging = false;
            m_verticalBar.bIsDragging   = false;
        }

        Bounds2D Scrollable::getScrollBarXTrack() const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result = m_horizontalBar.track;
            result.left += origin.x;
            result.right += origin.x;
            result.top += origin.y;
            result.bottom += origin.y;

            return result;
        }

        Bounds2D Scrollable::getScrollBarXThumb() const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result = m_horizontalBar.thumb;
            result.left += origin.x;
            result.right += origin.x;
            result.top += origin.y;
            result.bottom += origin.y;

            return result;
        }

        Bounds2D Scrollable::getScrollBarYTrack() const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result = m_verticalBar.track;
            result.left += origin.x;
            result.right += origin.x;
            result.top += origin.y;
            result.bottom += origin.y;

            return result;
        }

        Bounds2D Scrollable::getScrollBarYThumb() const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result = m_verticalBar.thumb;
            result.left += origin.x;
            result.right += origin.x;
            result.top += origin.y;
            result.bottom += origin.y;

            return result;
        }

        void Scrollable::clampScroll()
        {
            const Vec2 maxScroll = getScrollMax();

            if (canScrollX())
            {
                m_currentPosition.x = std::clamp(m_currentPosition.x, 0.0f, maxScroll.x);
            }
            else
            {
                m_currentPosition.x = 0.0f;
            }

            if (canScrollY())
            {
                m_currentPosition.y = std::clamp(m_currentPosition.y, 0.0f, maxScroll.y);
            }
            else
            {
                m_currentPosition.y = 0.0f;
            }
        }

        void Scrollable::refreshScrollBars()
        {
            m_horizontalBar.bIsVisible = canScrollX();
            m_verticalBar.bIsVisible   = canScrollY();

            const float thickness = std::max(6.0f, m_style.font.size.get() * 0.45f);
            const float minThumb  = std::max(thickness * 2.0f, 16.0f);
            const Vec2  maxScroll = getScrollMax();

            if (m_verticalBar.bIsVisible)
            {
                const float trackSize = std::max(0.0f, m_size.y - (m_horizontalBar.bIsVisible ? thickness : 0.0f));
                const float content   = m_size.y + maxScroll.y;
                const float thumbHi   = trackSize;
                const float thumbLo   = std::min(minThumb, thumbHi);
                const float thumbSize = std::clamp(trackSize * (m_size.y / std::max(content, 1.0f)), thumbLo, thumbHi);
                const float travel    = std::max(0.0f, trackSize - thumbSize);
                const float ratio     = maxScroll.y > 0.0f ? (m_currentPosition.y / maxScroll.y) : 0.0f;

                m_verticalBar.track.left   = std::max(0.0f, m_size.x - thickness);
                m_verticalBar.track.right  = m_size.x;
                m_verticalBar.track.top    = 0.0f;
                m_verticalBar.track.bottom = trackSize;

                m_verticalBar.thumb.left   = m_verticalBar.track.left;
                m_verticalBar.thumb.right  = m_verticalBar.track.right;
                m_verticalBar.thumb.top    = travel * ratio;
                m_verticalBar.thumb.bottom = m_verticalBar.thumb.top + thumbSize;
            }

            if (m_horizontalBar.bIsVisible)
            {
                const float trackSize = std::max(0.0f, m_size.x - (m_verticalBar.bIsVisible ? thickness : 0.0f));
                const float content   = m_size.x + maxScroll.x;
                const float thumbHi   = trackSize;
                const float thumbLo   = std::min(minThumb, thumbHi);
                const float thumbSize = std::clamp(trackSize * (m_size.x / std::max(content, 1.0f)), thumbLo, thumbHi);
                const float travel    = std::max(0.0f, trackSize - thumbSize);
                const float ratio     = maxScroll.x > 0.0f ? (m_currentPosition.x / maxScroll.x) : 0.0f;

                m_horizontalBar.track.left   = 0.0f;
                m_horizontalBar.track.right  = trackSize;
                m_horizontalBar.track.top    = std::max(0.0f, m_size.y - thickness);
                m_horizontalBar.track.bottom = m_size.y;

                m_horizontalBar.thumb.top    = m_horizontalBar.track.top;
                m_horizontalBar.thumb.bottom = m_horizontalBar.track.bottom;
                m_horizontalBar.thumb.left   = travel * ratio;
                m_horizontalBar.thumb.right  = m_horizontalBar.thumb.left + thumbSize;
            }

            if (m_verticalBar.bIsVisible)
            {
                ensureScrollBarParts(m_verticalBar);
            }

            if (m_horizontalBar.bIsVisible)
            {
                ensureScrollBarParts(m_horizontalBar);
            }

            if (m_verticalBar.bIsVisible)
            {
                syncScrollBarPart(m_verticalBar.trackBar, m_verticalBar.track, SCROLL_BAR_TRACK_COLOR, 0.0f);
                syncScrollBarPart(m_verticalBar.thumbBar, m_verticalBar.thumb, SCROLL_BAR_THUMB_COLOR, 0.1f);
            }
            else if (m_verticalBar.trackBar)
            {
                m_verticalBar.trackBar->hide();
                m_verticalBar.thumbBar->hide();
            }

            if (m_horizontalBar.bIsVisible)
            {
                syncScrollBarPart(m_horizontalBar.trackBar, m_horizontalBar.track, SCROLL_BAR_TRACK_COLOR, 0.0f);
                syncScrollBarPart(m_horizontalBar.thumbBar, m_horizontalBar.thumb, SCROLL_BAR_THUMB_COLOR, 0.1f);
            }
            else if (m_horizontalBar.trackBar)
            {
                m_horizontalBar.trackBar->hide();
                m_horizontalBar.thumbBar->hide();
            }
        }

        void Scrollable::ensureScrollBarParts(ScrollBarProperties& inBar)
        {
            auto makePart = [this]() -> ScrollBar*
            {
                ScrollBar* part = new ScrollBar();
                part->setRoot(m_root);
                part->setParent(this);
                part->setStyleFile(m_styleFile);

                return part;
            };

            if (!inBar.trackBar)
            {
                inBar.trackBar = makePart();
                markFlatDirty();
            }

            if (!inBar.thumbBar)
            {
                inBar.thumbBar = makePart();
                markFlatDirty();
            }
        }

        void Scrollable::syncScrollBarPart(
            ScrollBar* inPart, const Bounds2D& inBox, const Color::Rgba& inColor, float inDepthBias
        )
        {
            if (!inPart)
            {
                return;
            }

            inPart->configure(inBox, inColor, inDepthBias);
        }
    }
}
