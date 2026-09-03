#include "Chicane/Grid/Component/Dock.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "Chicane/Box/Font.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Window/Cursor.hpp"

#include "Chicane/Grid/Component/Button.hpp"
#include "Chicane/Grid/Component/Dock/Drop.hpp"
#include "Chicane/Grid/Component/Dock/Handle.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        const Color::Rgba kDropColor(11, 153, 125, 90);

        Dock::Dock(const pugi::xml_node& inNode)
            : Component(inNode),
              m_regions({}),
              m_drop(new DockDrop()),
              m_resize({}),
              m_drag({})
        {
            importStyleFile("Assets/Engine/UI/Components/Dock.decal");

            m_drop->setParent(this);
        }

        Dock::~Dock()
        {
            delete m_drop;
            m_drop = nullptr;
        }

        bool Dock::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (m_resize.panel)
                {
                    endResize();

                    return true;
                }

                if (m_drag.panel)
                {
                    endDrag();

                    return true;
                }

                return false;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left)
                {
                    return false;
                }

                DockPanel* panel = nullptr;
                if (hitResize(event.location, panel))
                {
                    beginResize(panel, event.location);

                    return true;
                }

                if (hitHandle(event.location, panel))
                {
                    beginDrag(panel, event.location);

                    return true;
                }

                return false;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);

                if (m_resize.panel)
                {
                    updateResize(event.location);

                    return true;
                }

                if (m_drag.panel)
                {
                    updateDrag(event.location);

                    return true;
                }

                refreshResizeCursor(event.location);

                return false;
            }

            return false;
        }

        void Dock::tick(float inDeltaTime)
        {
            Component::tick(inDeltaTime);

            refreshDrop();
        }

        std::vector<Component*> Dock::getChildrenFlat() const
        {
            std::vector<Component*> result = Component::getChildrenFlat();

            if (m_drop)
            {
                result.push_back(m_drop);
            }

            return result;
        }

        const DockRegion* Dock::findRegion(const DockPanel* inPanel) const
        {
            if (!inPanel)
            {
                return nullptr;
            }

            const auto found = m_regions.find(inPanel);
            if (found == m_regions.end())
            {
                return nullptr;
            }

            return &found->second;
        }

        void Dock::refreshSize()
        {
            Component::refreshSize();

            layout();

            if (m_resize.panel)
            {
                applyResizeCursor(m_resize.panel->getSide());
            }
            else if (m_drag.panel && m_drag.bActive)
            {
                applyDragCursor();
            }
        }

        DockPanel* Dock::asPanel(Component* inComponent) const
        {
            if (!inComponent || !inComponent->getTag().equals(DockPanel::TAG_ID))
            {
                return nullptr;
            }

            return static_cast<DockPanel*>(inComponent);
        }

        void Dock::collectPanels(
            std::vector<DockPanel*>& outEdges, std::vector<DockPanel*>& outFills, std::vector<DockPanel*>& outFloats
        ) const
        {
            outEdges.clear();
            outFills.clear();
            outFloats.clear();

            for (Component* child : m_children)
            {
                DockPanel* panel = asPanel(child);
                if (!panel || !panel->isDisplayable())
                {
                    continue;
                }

                if (panel->isFloating())
                {
                    outFloats.push_back(panel);

                    continue;
                }

                if (panel->isFill())
                {
                    outFills.push_back(panel);

                    continue;
                }

                outEdges.push_back(panel);
            }
        }

        void Dock::layout()
        {
            m_regions.clear();

            const Vec2 content(
                std::max(0.0f, m_size.x - m_style.insetHorizontal()),
                std::max(0.0f, m_size.y - m_style.insetVertical())
            );

            Bounds2D remaining;
            remaining.set(0.0f, 0.0f, content.y, content.x);

            std::vector<DockPanel*> edges;
            std::vector<DockPanel*> fills;
            std::vector<DockPanel*> floats;
            collectPanels(edges, fills, floats);

            for (DockPanel* panel : edges)
            {
                panel->refreshAttributes();
                claim(panel, remaining, content);
            }

            for (DockPanel* panel : fills)
            {
                panel->refreshAttributes();
            }

            assignFill(fills, remaining);

            for (DockPanel* panel : floats)
            {
                panel->refreshAttributes();
                assignFloat(panel);
            }
        }

        void Dock::claim(DockPanel* inPanel, Bounds2D& outRemaining, const Vec2& inContent)
        {
            const DockSide side   = inPanel->getSide();
            const float    extent = resolveExtent(inPanel, outRemaining, inContent);

            DockRegion region;

            switch (side)
            {
            case DockSide::Left: {
                const float leftover = std::max(0.0f, outRemaining.right - (outRemaining.left + extent));
                const float gap      = resolveGap(side, leftover);

                region.box.set(outRemaining.top, outRemaining.left, outRemaining.bottom, outRemaining.left + extent);
                region.gap.set(outRemaining.top, region.box.right, outRemaining.bottom, region.box.right + gap);
                region.bCanResize = inPanel->isResizable() && gap > 0.0f;
                outRemaining.left = region.box.right + gap;

                break;
            }

            case DockSide::Right: {
                const float leftover = std::max(0.0f, (outRemaining.right - extent) - outRemaining.left);
                const float gap      = resolveGap(side, leftover);

                region.box.set(outRemaining.top, outRemaining.right - extent, outRemaining.bottom, outRemaining.right);
                region.gap.set(outRemaining.top, region.box.left - gap, outRemaining.bottom, region.box.left);
                region.bCanResize  = inPanel->isResizable() && gap > 0.0f;
                outRemaining.right = region.box.left - gap;

                break;
            }

            case DockSide::Top: {
                const float leftover = std::max(0.0f, outRemaining.bottom - (outRemaining.top + extent));
                const float gap      = resolveGap(side, leftover);

                region.box.set(outRemaining.top, outRemaining.left, outRemaining.top + extent, outRemaining.right);
                region.gap.set(region.box.bottom, outRemaining.left, region.box.bottom + gap, outRemaining.right);
                region.bCanResize = inPanel->isResizable() && gap > 0.0f;
                outRemaining.top  = region.box.bottom + gap;

                break;
            }

            case DockSide::Bottom: {
                const float leftover = std::max(0.0f, (outRemaining.bottom - extent) - outRemaining.top);
                const float gap      = resolveGap(side, leftover);

                region.box
                    .set(outRemaining.bottom - extent, outRemaining.left, outRemaining.bottom, outRemaining.right);
                region.gap.set(region.box.top - gap, outRemaining.left, region.box.top, outRemaining.right);
                region.bCanResize   = inPanel->isResizable() && gap > 0.0f;
                outRemaining.bottom = region.box.top - gap;

                break;
            }

            default:
                return;
            }

            m_regions.insert_or_assign(inPanel, region);
        }

        void Dock::assignFill(const std::vector<DockPanel*>& inPanels, const Bounds2D& inRemaining)
        {
            if (inPanels.empty() || inRemaining.isEmpty())
            {
                return;
            }

            const float width   = inRemaining.right - inRemaining.left;
            const float height  = inRemaining.bottom - inRemaining.top;
            const bool  bSplitX = width >= height;
            const float gap     = std::max(0.0f, bSplitX ? m_style.gap.left.get() : m_style.gap.top.get());
            const float usable  = std::max(
                0.0f, (bSplitX ? width : height) - gap * static_cast<float>(inPanels.size() > 0 ? inPanels.size() - 1 : 0)
            );
            const float slice = usable / static_cast<float>(inPanels.size());

            for (std::size_t i = 0; i < inPanels.size(); ++i)
            {
                DockRegion region;
                const float offset = (slice + gap) * static_cast<float>(i);

                if (bSplitX)
                {
                    const float left  = inRemaining.left + offset;
                    const float right = (i + 1 == inPanels.size()) ? inRemaining.right : left + slice;
                    region.box.set(inRemaining.top, left, inRemaining.bottom, right);
                }
                else
                {
                    const float top    = inRemaining.top + offset;
                    const float bottom = (i + 1 == inPanels.size()) ? inRemaining.bottom : top + slice;
                    region.box.set(top, inRemaining.left, bottom, inRemaining.right);
                }

                m_regions.insert_or_assign(inPanels.at(i), region);
            }
        }

        void Dock::assignFloat(DockPanel* inPanel)
        {
            Vec2 size = inPanel->getFloatSize();
            if (size.x <= 0.0f || size.y <= 0.0f)
            {
                size = inPanel->getSize();
            }

            if (size.x <= 0.0f)
            {
                size.x = parseExtent("20em", SizeDirection::Horizontal, m_size);
            }

            if (size.y <= 0.0f)
            {
                size.y = parseExtent("16em", SizeDirection::Vertical, m_size);
            }

            const Vec2 position = inPanel->getFloatPosition();

            DockRegion region;
            region.box.set(position.y, position.x, position.y + size.y, position.x + size.x);
            m_regions.insert_or_assign(inPanel, region);
        }

        void Dock::refreshDrop()
        {
            if (!m_drop)
            {
                return;
            }

            m_drop->setRoot(m_root);
            m_drop->setParent(this);
            m_drop->setStyleFile(m_styleFile);

            if (!m_drag.panel || !m_drag.bActive || m_drag.drop == DockSide::Float)
            {
                m_drop->hide();

                return;
            }

            Bounds2D box = getDropPreview(m_drag.drop);
            box.left += m_style.insetLeft();
            box.right += m_style.insetLeft();
            box.top += m_style.insetTop();
            box.bottom += m_style.insetTop();

            m_drop->configure(box, kDropColor);
        }

        bool Dock::hitHandle(const Vec2& inLocation, DockPanel*& outPanel) const
        {
            outPanel = nullptr;

            Component* node = getHitAt(inLocation);
            if (node && node->getTag().equals(Button::TAG_ID))
            {
                DockPanel* panel = DockPanel::findFrom(node);
                if (!panel || !panel->isAssignedHandle(node))
                {
                    return false;
                }
            }

            while (node && node != this)
            {
                DockPanel* panel     = DockPanel::findFrom(node);
                const bool bAssigned = panel && panel->isAssignedHandle(node);
                const bool bOverlay  = node->getTag().equals(DockHandle::TAG_ID);
                if (bAssigned || bOverlay)
                {
                    if (panel && panel->isGrabbable() && panel->getParent() == this && panel->isDisplayable() &&
                        (bAssigned || !panel->hasAssignedHandle()))
                    {
                        outPanel = panel;

                        return true;
                    }
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return false;
        }

        bool Dock::hitResize(const Vec2& inLocation, DockPanel*& outPanel) const
        {
            outPanel = nullptr;

            for (Component* child : m_children)
            {
                DockPanel* panel = asPanel(child);
                if (!panel)
                {
                    continue;
                }

                const DockRegion* region = findRegion(panel);
                if (!region || !region->bCanResize)
                {
                    continue;
                }

                if (!getGapBounds(*region).contains(inLocation))
                {
                    continue;
                }

                outPanel = panel;

                return true;
            }

            return false;
        }

        Bounds2D Dock::getGapBounds(const DockRegion& inRegion) const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result = inRegion.gap;
            result.left += origin.x + m_style.insetLeft();
            result.right += origin.x + m_style.insetLeft();
            result.top += origin.y + m_style.insetTop();
            result.bottom += origin.y + m_style.insetTop();

            return result;
        }

        Bounds2D Dock::getContentBounds() const
        {
            const Vec2 origin = getDrawPosition();

            Bounds2D result;
            result.set(
                origin.y + m_style.insetTop(),
                origin.x + m_style.insetLeft(),
                origin.y + m_style.insetTop() + std::max(0.0f, m_size.y - m_style.insetVertical()),
                origin.x + m_style.insetLeft() + std::max(0.0f, m_size.x - m_style.insetHorizontal())
            );

            return result;
        }

        Bounds2D Dock::getDropPreview(DockSide inSide) const
        {
            const Vec2 content(
                std::max(0.0f, m_size.x - m_style.insetHorizontal()),
                std::max(0.0f, m_size.y - m_style.insetVertical())
            );

            const float bandX = std::max(DROP_BAND_MIN, content.x * DROP_BAND_RATIO);
            const float bandY = std::max(DROP_BAND_MIN, content.y * DROP_BAND_RATIO);

            Bounds2D result;
            result.set(0.0f, 0.0f, content.y, content.x);

            switch (inSide)
            {
            case DockSide::Left:
                result.right = std::min(content.x, bandX);

                break;

            case DockSide::Right:
                result.left = std::max(0.0f, content.x - bandX);

                break;

            case DockSide::Top:
                result.bottom = std::min(content.y, bandY);

                break;

            case DockSide::Bottom:
                result.top = std::max(0.0f, content.y - bandY);

                break;

            case DockSide::Fill:
                result.left += bandX * 0.5f;
                result.right -= bandX * 0.5f;
                result.top += bandY * 0.5f;
                result.bottom -= bandY * 0.5f;

                break;

            default:
                result.set(0.0f, 0.0f, 0.0f, 0.0f);

                break;
            }

            return result;
        }

        void Dock::beginResize(DockPanel* inPanel, const Vec2& inLocation)
        {
            const DockRegion* region = findRegion(inPanel);
            if (!region)
            {
                return;
            }

            const float extent = isHorizontal(inPanel->getSide()) ? (region->box.right - region->box.left)
                                                                  : (region->box.bottom - region->box.top);

            m_resize.panel       = inPanel;
            m_resize.cursor      = inLocation;
            m_resize.startExtent = extent;

            applyResizeCursor(inPanel->getSide());
        }

        void Dock::updateResize(const Vec2& inLocation)
        {
            if (!m_resize.panel)
            {
                return;
            }

            const Vec2 delta  = inLocation - m_resize.cursor;
            float      extent = m_resize.startExtent;

            switch (m_resize.panel->getSide())
            {
            case DockSide::Left:
                extent += delta.x;

                break;

            case DockSide::Right:
                extent -= delta.x;

                break;

            case DockSide::Top:
                extent += delta.y;

                break;

            case DockSide::Bottom:
                extent -= delta.y;

                break;

            default:
                return;
            }

            m_resize.panel->setExtent(extent);
        }

        void Dock::endResize()
        {
            m_resize = {};
            clearCursor();
        }

        void Dock::applyResizeCursor(DockSide inSide)
        {
            if (isHorizontal(inSide))
            {
                m_style.cursor.setRaw(Style::CURSOR_TYPE_EW_RESIZE);
                m_style.cursor.set(WindowCursor::EwResize);

                return;
            }

            m_style.cursor.setRaw(Style::CURSOR_TYPE_NS_RESIZE);
            m_style.cursor.set(WindowCursor::NsResize);
        }

        void Dock::refreshResizeCursor(const Vec2& inLocation)
        {
            DockPanel* panel = nullptr;
            if (hitResize(inLocation, panel))
            {
                applyResizeCursor(panel->getSide());

                return;
            }

            if (m_style.cursor.get() == WindowCursor::EwResize || m_style.cursor.get() == WindowCursor::NsResize)
            {
                clearCursor();
            }
        }

        void Dock::beginDrag(DockPanel* inPanel, const Vec2& inLocation)
        {
            if (!inPanel)
            {
                return;
            }

            m_drag.panel   = inPanel;
            m_drag.cursor  = inLocation;
            m_drag.grab    = inLocation - inPanel->getDrawPosition();
            m_drag.bActive = inPanel->isFloating();
            m_drag.drop    = inPanel->getSide();

            if (m_drag.bActive)
            {
                applyDragCursor();
            }
        }

        void Dock::updateDrag(const Vec2& inLocation)
        {
            if (!m_drag.panel)
            {
                return;
            }

            if (!m_drag.bActive)
            {
                const float dx = inLocation.x - m_drag.cursor.x;
                const float dy = inLocation.y - m_drag.cursor.y;

                if ((dx * dx) + (dy * dy) < (DRAG_THRESHOLD * DRAG_THRESHOLD))
                {
                    return;
                }

                m_drag.bActive = true;
                m_drag.panel->setSide(DockSide::Float);
                m_drag.panel->clearExtent();
                raise(m_drag.panel);
            }

            const Vec2 origin(getDrawPosition().x + m_style.insetLeft(), getDrawPosition().y + m_style.insetTop());
            const Vec2 content(
                std::max(0.0f, m_size.x - m_style.insetHorizontal()),
                std::max(0.0f, m_size.y - m_style.insetVertical())
            );

            Vec2 size = m_drag.panel->getFloatSize();
            if (size.x <= 0.0f || size.y <= 0.0f)
            {
                size = m_drag.panel->getSize();
                size.x =
                    std::clamp(size.x, HANDLE_THICKNESS * 8.0f, std::max(HANDLE_THICKNESS * 8.0f, content.x * 0.5f));
                size.y =
                    std::clamp(size.y, HANDLE_THICKNESS * 8.0f, std::max(HANDLE_THICKNESS * 8.0f, content.y * 0.5f));
                m_drag.panel->setFloatSize(size);
                m_drag.grab.x = std::clamp(m_drag.grab.x, 0.0f, size.x);
                m_drag.grab.y = std::clamp(m_drag.grab.y, 0.0f, size.y);
            }

            Vec2 position = inLocation - origin - m_drag.grab;
            position.x =
                std::clamp(position.x, HANDLE_THICKNESS - size.x, std::max(0.0f, content.x - HANDLE_THICKNESS));
            position.y =
                std::clamp(position.y, HANDLE_THICKNESS - size.y, std::max(0.0f, content.y - HANDLE_THICKNESS));

            m_drag.panel->setFloatPosition(position);
            m_drag.drop = pickDropSide(inLocation);

            applyDragCursor();
        }

        void Dock::endDrag()
        {
            if (m_drag.panel && m_drag.bActive)
            {
                if (m_drag.drop == DockSide::Float)
                {
                    m_drag.panel->setSide(DockSide::Float);
                }
                else
                {
                    m_drag.panel->clearExtent();
                    m_drag.panel->setFloatSize(Vec2::Zero());
                    m_drag.panel->setSide(m_drag.drop);
                }

                raise(m_drag.panel);
            }

            m_drag = {};
            clearCursor();

            if (m_drop)
            {
                m_drop->hide();
            }
        }

        void Dock::applyDragCursor()
        {
            m_style.cursor.setRaw(Style::CURSOR_TYPE_GRABBING);
            m_style.cursor.set(WindowCursor::Grabbing);
        }

        void Dock::clearCursor()
        {
            m_style.cursor.setRaw("");
            m_style.cursor.set(WindowCursor::Default);
        }

        void Dock::raise(DockPanel* inPanel)
        {
            if (!inPanel)
            {
                return;
            }

            const auto found = std::find(m_children.begin(), m_children.end(), inPanel);
            if (found == m_children.end())
            {
                return;
            }

            m_children.erase(found);
            m_children.push_back(inPanel);
        }

        DockSide Dock::pickDropSide(const Vec2& inLocation) const
        {
            const Bounds2D content = getContentBounds();
            if (!content.contains(inLocation))
            {
                return DockSide::Float;
            }

            const float width  = content.right - content.left;
            const float height = content.bottom - content.top;
            const float bandX  = std::max(DROP_BAND_MIN, width * DROP_BAND_RATIO);
            const float bandY  = std::max(DROP_BAND_MIN, height * DROP_BAND_RATIO);

            const float left   = inLocation.x - content.left;
            const float right  = content.right - inLocation.x;
            const float top    = inLocation.y - content.top;
            const float bottom = content.bottom - inLocation.y;

            DockSide side = DockSide::Fill;
            float    best = std::numeric_limits<float>::max();

            auto consider = [&](float inDistance, float inBand, DockSide inSide)
            {
                if (inDistance > inBand || inDistance >= best)
                {
                    return;
                }

                best = inDistance;
                side = inSide;
            };

            consider(left, bandX, DockSide::Left);
            consider(right, bandX, DockSide::Right);
            consider(top, bandY, DockSide::Top);
            consider(bottom, bandY, DockSide::Bottom);

            return side;
        }

        float Dock::resolveGap(DockSide inSide, float inLeftover) const
        {
            float gap = 0.0f;

            switch (inSide)
            {
            case DockSide::Left:
                gap = m_style.gap.left.get();

                break;

            case DockSide::Right:
                gap = m_style.gap.right.get();

                break;

            case DockSide::Top:
                gap = m_style.gap.top.get();

                break;

            case DockSide::Bottom:
                gap = m_style.gap.bottom.get();

                break;

            default:
                return 0.0f;
            }

            return std::clamp(gap, 0.0f, std::max(0.0f, inLeftover));
        }

        float Dock::resolveExtent(DockPanel* inPanel, const Bounds2D& inRemaining, const Vec2& inContent) const
        {
            const SizeDirection direction = axisOf(inPanel->getSide());
            const float         available = isHorizontal(inPanel->getSide()) ? (inRemaining.right - inRemaining.left)
                                                                             : (inRemaining.bottom - inRemaining.top);

            const float minExtent = resolveLimit(
                inPanel->getMinSizeValue(),
                direction,
                inContent,
                parseExtent(DockPanel::DEFAULT_MIN_SIZE, direction, inContent)
            );
            const float maxExtent = resolveLimit(inPanel->getMaxSizeValue(), direction, inContent, available);

            float extent = available;

            if (inPanel->hasExtent())
            {
                extent = inPanel->getExtent();
            }
            else
            {
                const String size =
                    inPanel->getSizeValue().isEmpty() ? String(DockPanel::DEFAULT_SIZE) : inPanel->getSizeValue();
                extent = parseExtent(size, direction, inContent);
            }

            return std::clamp(extent, std::min(minExtent, available), std::min(maxExtent, available));
        }

        float Dock::resolveLimit(
            const String& inValue, SizeDirection inDirection, const Vec2& inContent, float inFallback
        ) const
        {
            if (inValue.isEmpty())
            {
                return inFallback;
            }

            return parseExtent(inValue, inDirection, inContent);
        }

        float Dock::parseExtent(const String& inValue, SizeDirection inDirection, const Vec2& inContent) const
        {
            if (inValue.isEmpty())
            {
                return 0.0f;
            }

            Size parser;
            parser.setFontSize(m_style.font.size.get() > 0.0f ? m_style.font.size.get() : Box::Font::BASE_SIZE);
            parser.setParent(inContent);
            parser.setRoot(hasRoot() ? getRoot()->getSize() : inContent);
            parser.setTextParser([this](const String& inText) { return parseText(inText); });

            return std::max(0.0f, parser.parse(inValue, inDirection));
        }

        SizeDirection Dock::axisOf(DockSide inSide) const
        {
            return isHorizontal(inSide) ? SizeDirection::Horizontal : SizeDirection::Vertical;
        }

        bool Dock::isHorizontal(DockSide inSide) const
        {
            return inSide == DockSide::Left || inSide == DockSide::Right;
        }
    }
}
