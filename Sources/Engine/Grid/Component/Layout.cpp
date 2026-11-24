#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

#include "Chicane/Grid/Component/LayoutMetrics.hpp"
#include "Chicane/Grid/Component/Loading.hpp"
#include "Chicane/Grid/Component/Projection.hpp"
#include "Chicane/Grid/Component/Scope.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/StyleMatch.hpp"
#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Component/Script.hpp"

namespace Chicane
{
    namespace Grid
    {
        static bool isNonFillPercent(const StyleSize& inSize)
        {
            const String raw = inSize.value.getRaw().trim();

            return raw.endsWith("%") && !raw.equals("100%") && !raw.equals("100.0%");
        }

        static bool isHeightAuto(const Style& inStyle)
        {
            return inStyle.height.isAuto();
        }

        static bool isWidthAuto(const Style& inStyle)
        {
            return inStyle.width.isAuto();
        }

        static bool isWidthIntrinsicAuto(const Style& inStyle, const Style& inParentStyle)
        {
            return isWidthAuto(inStyle) || (isNonFillPercent(inStyle.width) &&
                                            (isWidthAuto(inParentStyle) || isNonFillPercent(inParentStyle.width)));
        }

        static bool isHeightIntrinsicAuto(const Style& inStyle, const Style& inParentStyle)
        {
            return isHeightAuto(inStyle) || (isNonFillPercent(inStyle.height) &&
                                             (isHeightAuto(inParentStyle) || isNonFillPercent(inParentStyle.height)));
        }

        static bool isFlexNowrap(const Style& inStyle)
        {
            return inStyle.isDisplay(StyleDisplay::Flex) && inStyle.flex.wrap.get() == StyleFlexWrap::NoWrap;
        }

        static bool isMainFillPercent(const Component* inComponent, bool bIsRow)
        {
            if (!inComponent)
            {
                return false;
            }

            const Style& style = inComponent->getStyle();

            return bIsRow ? style.isFillPercent(style.width.value.getRaw())
                          : style.isFillPercent(style.height.value.getRaw());
        }

        static void applyLaidOutRadius(Style& outStyle, const Vec2& inSize)
        {
            outStyle.radius.refresh();
            outStyle.radius.constrain(inSize.x, inSize.y);
        }

        static bool hasLayoutTween(const StyleAnimator& inAnimator)
        {
            return has(inAnimator.getDirty(), StylePropertyDirty::Layout);
        }

        static Vec2 innerLayoutSize(const Component* inBox)
        {
            if (!inBox)
            {
                return Vec2::sZero();
            }

            const Vec2 content = inBox->getInnerLayoutSize();

            return {std::max(0.0f, content.x), std::max(0.0f, content.y)};
        }

        static float percentOfBasis(const String& inRaw, float inBasis, SizeDirection inDirection)
        {
            if (inBasis <= 0.0f)
            {
                return -1.0f;
            }

            const String raw = inRaw.trim();
            if (!raw.endsWith(Size::PERCENTAGE_UNIT))
            {
                return -1.0f;
            }

            Size parser;
            parser.setParent(Vec2(inBasis, inBasis));

            return std::max(0.0f, parser.parse(raw, inDirection));
        }

        static bool stretchesAutoWidth(const Component* inBox)
        {
            if (!inBox || !inBox->hasParent())
            {
                return false;
            }

            const Style& style = inBox->getStyle();
            if (!isWidthAuto(style) || style.isPosition(StylePosition::Absolute))
            {
                return false;
            }

            const Style& parent = inBox->getParent()->getStyle();
            if (parent.isDisplay(StyleDisplay::Flex) && parent.flex.direction.get() == StyleFlexDirection::Row)
            {
                return false;
            }

            return !style.margin.left.isRaw(Size::AUTO_KEYWORD) && !style.margin.right.isRaw(Size::AUTO_KEYWORD);
        }

        static float flexWrapMainSize(const Component* inBox, bool bIsRow)
        {
            if (!inBox)
            {
                return 0.0f;
            }

            const Vec2  inner = innerLayoutSize(inBox);
            const float main  = bIsRow ? inner.x : inner.y;
            if (main > 0.01f)
            {
                return main;
            }

            const Component* containing = inBox->getContainingBlock();
            if (!containing)
            {
                return 0.0f;
            }

            const Vec2   available = innerLayoutSize(containing);
            float        basis     = bIsRow ? available.x : available.y;
            const Style& style     = inBox->getStyle();

            const bool bRow = static_cast<bool>(bIsRow);

            if (bRow)
            {
                if (isWidthAuto(style) && !stretchesAutoWidth(inBox))
                {
                    return 0.0f;
                }

                const bool bWidthAutoOrFillPercent =
                    static_cast<bool>(isWidthAuto(style) || style.isFillPercent(style.width.value.getRaw()));

                if (bWidthAutoOrFillPercent)
                {
                    basis -= (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get());
                    basis -= (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get());
                }

                if (!bWidthAutoOrFillPercent)
                {
                    const float parsed = percentOfBasis(style.width.value.getRaw(), basis, SizeDirection::Horizontal);
                    if (parsed >= 0.0f)
                    {
                        basis = parsed;
                    }
                }

                basis -= style.insetHorizontal();
            }

            if (!bRow)
            {
                if (isHeightAuto(style))
                {
                    return 0.0f;
                }

                const bool bFillPercent = static_cast<bool>(style.isFillPercent(style.height.value.getRaw()));

                if (bFillPercent)
                {
                    basis -= (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get());
                    basis -= (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get());
                }

                if (!bFillPercent)
                {
                    const float parsed = percentOfBasis(style.height.value.getRaw(), basis, SizeDirection::Vertical);
                    if (parsed >= 0.0f)
                    {
                        basis = parsed;
                    }
                }

                basis -= style.insetVertical();
            }

            return std::max(0.0f, basis);
        }

        static String expandStyleBinding(const String& inValue)
        {
            const std::size_t ref = inValue.find("ref(");
            if (ref == String::npos)
            {
                return inValue;
            }

            const std::size_t open = ref + 3;

            std::uint32_t depth = 0;
            std::size_t   close = String::npos;
            for (std::size_t i = open; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);
                if (character == METHOD_PARAMS_OPENING)
                {
                    depth++;

                    continue;
                }

                if (character != METHOD_PARAMS_CLOSING)
                {
                    continue;
                }

                depth--;
                if (depth == 0)
                {
                    close = i;

                    break;
                }
            }

            if (close == String::npos)
            {
                return inValue;
            }

            String converted;
            converted.append(inValue.substr(0, ref));
            converted.append(REFERENCE_VALUE_OPENING);
            converted.append(inValue.substr(open + 1, close - open - 1));
            converted.append(REFERENCE_VALUE_CLOSING);
            converted.append(inValue.substr(close + 1));

            return converted;
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            if (hasParent() && m_parent->hasFlag(ComponentDirty::LaidOut) &&
                !style.isPosition(StylePosition::Absolute) && isFlexNowrap(m_parent->getStyle()) &&
                (style.isFillPercent(style.width.value.getRaw()) || style.isFillPercent(style.height.value.getRaw())))
            {
                style.resolveFillPercent(m_parent->getRemainingContentSize(this));
            }

            const bool bIsWidthAuto  = style.width.isAuto();
            const bool bIsHeightAuto = style.height.isAuto();

            float width  = style.width.value.get();
            float height = style.height.value.get();

            const bool bStretchWidth = bIsWidthAuto && stretchesAutoWidth(this);
            if (bStretchWidth)
            {
                const Component* box       = getContainingBlock();
                const float      available = innerLayoutSize(box).x;
                const float      horizontalMargin =
                    (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get()) +
                    (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get());

                width = std::max(0.0f, available - horizontalMargin);
            }

            if (bIsWidthAuto || bIsHeightAuto)
            {
                if (!bIsWidthAuto || bStretchWidth)
                {
                    m_size.x = width;
                }

                if (!bIsHeightAuto)
                {
                    m_size.y = height;
                }

                const Vec2 content = getChildrenContentSize();

                if (bIsWidthAuto && !bStretchWidth)
                {
                    width = content.x + style.insetHorizontal();
                }

                if (bIsHeightAuto)
                {
                    height = content.y + style.insetVertical();
                }
            }

            style.clampSize(width, height);
            setSize(width, height);
            setFlag(ComponentDirty::Insets);
        }

        void Component::refreshPosition()
        {
            if (style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            setPosition(0.0f, 0.0f);

            float marginLeft   = style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get();
            float marginRight  = style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get();
            float marginTop    = style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get();
            float marginBottom = style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get();

            const Vec2 startPadding(style.insetLeft(), style.insetTop());

            const Vec2 contentSize = getContentSize();

            const float usedWidth  = contentSize.x + style.insetHorizontal();
            const float usedHeight = contentSize.y + style.insetVertical();

            if (hasParent() && !isRoot())
            {
                const Component* box = style.isPosition(StylePosition::Absolute) ? getContainingBlock() : m_parent;
                const Style&     parentStyle = m_parent->getStyle();
                const Vec2       available   = innerLayoutSize(box);

                const bool bIsLeftAuto     = style.margin.left.isRaw(Size::AUTO_KEYWORD);
                const bool bIsRightAuto    = style.margin.right.isRaw(Size::AUTO_KEYWORD);
                const bool bIsTopAuto      = style.margin.top.isRaw(Size::AUTO_KEYWORD);
                const bool bIsBottomAuto   = style.margin.bottom.isRaw(Size::AUTO_KEYWORD);
                const bool bIsParentCenter = parentStyle.align.get() == StyleAlignment::Center;
                const bool bIsParentFlex   = parentStyle.isDisplay(StyleDisplay::Flex);
                const bool bIsParentRow = bIsParentFlex && parentStyle.flex.direction.get() == StyleFlexDirection::Row;

                float leftoverW = available.x - usedWidth - marginLeft - marginRight;

                if (!style.isPosition(StylePosition::Absolute) && bIsParentRow)
                {
                    leftoverW = m_parent->getPosition().x + parentStyle.insetLeft() + innerLayoutSize(m_parent).x -
                                m_parent->getCursor().x - usedWidth - marginLeft - marginRight;

                    const float lineStart = m_parent->getPosition().x + parentStyle.insetLeft();
                    if (m_parent->getCursor().x > lineStart)
                    {
                        leftoverW -= parentStyle.gap.left.get();
                    }

                    leftoverW -= m_parent->getTrailingMainSize(this);
                }

                leftoverW = std::max(0.0f, leftoverW);

                if (leftoverW > 0.0f)
                {
                    const bool bShareMainAuto =
                        !style.isPosition(StylePosition::Absolute) && bIsParentRow && (bIsLeftAuto || bIsRightAuto);
                    const int   mainAutos = bShareMainAuto ? m_parent->countTrailingMainAutoMargins(this) : 0;
                    const float mainShare = mainAutos > 0 ? leftoverW / static_cast<float>(mainAutos) : leftoverW;

                    const bool bShareMainAutoAndMainAutosPositive = static_cast<bool>(bShareMainAuto && mainAutos > 0);

                    if (bShareMainAutoAndMainAutosPositive)
                    {
                        if (bIsLeftAuto)
                        {
                            marginLeft = mainShare;
                        }

                        if (bIsRightAuto)
                        {
                            marginRight = mainShare;
                        }
                    }

                    const bool bParentCenterOrIsRightAuto =
                        !bShareMainAutoAndMainAutosPositive && (bIsParentCenter || (bIsLeftAuto && bIsRightAuto));

                    if (bParentCenterOrIsRightAuto)
                    {
                        marginLeft  = leftoverW * 0.5f;
                        marginRight = leftoverW * 0.5f;
                    }

                    const bool bLeftAuto =
                        !bShareMainAutoAndMainAutosPositive && !bParentCenterOrIsRightAuto && (bIsLeftAuto);

                    if (bLeftAuto)
                    {
                        marginLeft = leftoverW;
                    }

                    const bool bRightAuto = !bShareMainAutoAndMainAutosPositive && !bParentCenterOrIsRightAuto &&
                                            !bLeftAuto && (bIsRightAuto);

                    if (bRightAuto)
                    {
                        marginRight = leftoverW;
                    }
                }

                const bool bIsParentHeightAuto = parentStyle.height.isAuto();

                const bool bCanAutoVertical =
                    style.isPosition(StylePosition::Absolute) || bIsParentFlex || !bIsParentHeightAuto;

                if (bCanAutoVertical && (bIsParentCenter || bIsTopAuto || bIsBottomAuto))
                {
                    float leftoverH = available.y - usedHeight - marginTop - marginBottom;

                    if (!style.isPosition(StylePosition::Absolute) && bIsParentFlex && !bIsParentRow)
                    {
                        leftoverH = m_parent->getPosition().y + parentStyle.insetTop() + innerLayoutSize(m_parent).y -
                                    m_parent->getCursor().y - usedHeight - marginTop - marginBottom;

                        const float lineStart = m_parent->getPosition().y + parentStyle.insetTop();
                        if (m_parent->getCursor().y > lineStart)
                        {
                            leftoverH -= parentStyle.gap.top.get();
                        }

                        leftoverH -= m_parent->getTrailingMainSize(this);
                    }

                    leftoverH = std::max(0.0f, leftoverH);

                    if (leftoverH > 0.0f)
                    {
                        const bool  bShareMainAuto = !style.isPosition(StylePosition::Absolute) && bIsParentFlex &&
                                                     !bIsParentRow && (bIsTopAuto || bIsBottomAuto);
                        const int   mainAutos      = bShareMainAuto ? m_parent->countTrailingMainAutoMargins(this) : 0;
                        const float mainShare = mainAutos > 0 ? leftoverH / static_cast<float>(mainAutos) : leftoverH;

                        const bool bShareMainAutoAndMainAutosPositive =
                            static_cast<bool>(bShareMainAuto && mainAutos > 0);

                        if (bShareMainAutoAndMainAutosPositive)
                        {
                            if (bIsTopAuto)
                            {
                                marginTop = mainShare;
                            }

                            if (bIsBottomAuto)
                            {
                                marginBottom = mainShare;
                            }
                        }

                        const bool bParentCenterOrIsBottomAuto =
                            !bShareMainAutoAndMainAutosPositive && (bIsParentCenter || (bIsTopAuto && bIsBottomAuto));

                        if (bParentCenterOrIsBottomAuto)
                        {
                            marginTop    = leftoverH * 0.5f;
                            marginBottom = leftoverH * 0.5f;
                        }

                        const bool bTopAuto =
                            !bShareMainAutoAndMainAutosPositive && !bParentCenterOrIsBottomAuto && (bIsTopAuto);

                        if (bTopAuto)
                        {
                            marginTop = leftoverH;
                        }

                        const bool bBottomAuto = !bShareMainAutoAndMainAutosPositive && !bParentCenterOrIsBottomAuto &&
                                                 !bTopAuto && (bIsBottomAuto);

                        if (bBottomAuto)
                        {
                            marginBottom = leftoverH;
                        }
                    }
                }
            }

            if (isRoot() || style.isPosition(StylePosition::Absolute))
            {
                Vec2 origin = Vec2::sZero();

                if (!isRoot() && hasParent())
                {
                    const Component* containingBlock = getContainingBlock();
                    origin.x = containingBlock->getPosition().x + containingBlock->getStyle().insetLeft();
                    origin.y = containingBlock->getPosition().y + containingBlock->getStyle().insetTop();
                }

                setPosition(origin.x + marginLeft, origin.y + marginTop);
                addCursor(startPadding);

                return;
            }

            const Style& parentStyle = m_parent->getStyle();
            const Vec2   available   = innerLayoutSize(m_parent);

            switch (parentStyle.display.get())
            {
            case StyleDisplay::Flex: {
                const bool bIsRow   = parentStyle.flex.direction.get() == StyleFlexDirection::Row;
                const bool bCanWrap = parentStyle.flex.wrap.get() == StyleFlexWrap::Wrap;

                const float mainGap  = bIsRow ? parentStyle.gap.left.get() : parentStyle.gap.top.get();
                const float crossGap = bIsRow ? parentStyle.gap.top.get() : parentStyle.gap.left.get();

                const float itemMain =
                    bIsRow ? (usedWidth + marginLeft + marginRight) : (usedHeight + marginTop + marginBottom);
                const float itemCross =
                    bIsRow ? (usedHeight + marginTop + marginBottom) : (usedWidth + marginLeft + marginRight);

                const float lineStart       = bIsRow ? (m_parent->getPosition().x + parentStyle.insetLeft())
                                                     : (m_parent->getPosition().y + parentStyle.insetTop());
                const float lineLimit       = bIsRow ? (lineStart + available.x) : (lineStart + available.y);
                float       cursorMain      = bIsRow ? m_parent->getCursor().x : m_parent->getCursor().y;
                const bool  bHasLineStarted = cursorMain > lineStart;

                if (bHasLineStarted)
                {
                    const bool bWrapAndItemMainLineLimit =
                        static_cast<bool>(bCanWrap && (cursorMain + mainGap + itemMain) > lineLimit);

                    if (bWrapAndItemMainLineLimit)
                    {
                        const bool bRow = static_cast<bool>(bIsRow);

                        if (bRow)
                        {
                            m_parent->setCursor(lineStart, m_parent->getCursor().y + m_parent->m_scratch + crossGap);
                        }

                        if (!bRow)
                        {
                            m_parent->setCursor(m_parent->getCursor().x + m_parent->m_scratch + crossGap, lineStart);
                        }

                        m_parent->m_scratch = 0.0f;
                    }

                    const bool bRow2 = !bWrapAndItemMainLineLimit && (bIsRow);

                    if (bRow2)
                    {
                        m_parent->addCursor(mainGap, 0.0f);
                    }

                    if (!bWrapAndItemMainLineLimit && !bRow2)
                    {
                        m_parent->addCursor(0.0f, mainGap);
                    }
                }

                setPosition(m_parent->getCursor() + Vec2(marginLeft, marginTop));
                addCursor(startPadding);

                m_parent->m_scratch = std::max(m_parent->m_scratch, itemCross);

                const bool bRow3 = static_cast<bool>(bIsRow);

                if (bRow3)
                {
                    m_parent->addCursor(marginLeft + usedWidth + marginRight, 0.0f);
                }

                if (!bRow3)
                {
                    m_parent->addCursor(0.0f, marginTop + usedHeight + marginBottom);
                }

                break;
            }

            default:
                setPosition(m_parent->getCursor() + Vec2(marginLeft, marginTop));
                addCursor(startPadding);

                m_parent->addCursor(0.0f, m_size.y + marginBottom);

                break;
            }
        }

        void Component::tick(float inDeltaTime)
        {
            if (m_script)
            {
                m_script->tick(inDeltaTime);
            }

            m_animationDelta = inDeltaTime;
            setFlag(ComponentDirty::LaidOut, false);

            refresh();

            m_animationDelta = 0.0f;

            if (style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            if (isCulled() && !hasFlag(ComponentDirty::LaidOut) && m_animator.isIdle())
            {
                return;
            }

            onTick(inDeltaTime);

            for (std::size_t i = 0; i < m_children.size(); i++)
            {
                m_children.at(i)->tick(inDeltaTime);
            }

            if (!hasFlag(ComponentDirty::LaidOut))
            {
                return;
            }

            const bool bIsWidthAuto  = isWidthAuto(style);
            const bool bIsHeightAuto = isHeightAuto(style);

            if (bIsWidthAuto || bIsHeightAuto)
            {
                const Vec2 previous = getContentSize();

                refreshSize();

                if (m_parent && (std::abs(previous.x - getContentSize().x) > 0.01f ||
                                 std::abs(previous.y - getContentSize().y) > 0.01f))
                {
                    m_parent->markLayoutDirty();
                }
            }

            if (style.isPosition(StylePosition::Absolute))
            {
                refreshPosition();
            }

            reflowChildPositions();

            if (bIsHeightAuto)
            {
                const float previousHeight = getSize().y;

                refreshSize();

                if (std::abs(previousHeight - getSize().y) > 0.01f)
                {
                    if (style.isPosition(StylePosition::Absolute))
                    {
                        refreshPosition();
                    }

                    reflowChildPositions();

                    if (m_parent)
                    {
                        m_parent->markLayoutDirty();
                    }
                }
            }

            applyLaidOutRadius(style, m_size);
            refreshBounds();
            markPaintDirtySubtree();

            if (hasFlag(ComponentDirty::Layout))
            {
                std::function<void(Component*)> relayout;
                relayout = [&](Component* node)
                {
                    if (!node || !node->hasFlag(ComponentDirty::Layout))
                    {
                        return;
                    }

                    node->refresh();

                    const std::vector<Component*> children = node->getChildren();
                    for (Component* child : children)
                    {
                        const std::vector<Component*>& live = node->getChildren();
                        if (std::find(live.begin(), live.end(), child) == live.end())
                        {
                            continue;
                        }

                        relayout(child);
                    }
                };

                relayout(this);
            }
        }

        void Component::refresh()
        {
            const StyleDisplay displayBefore = style.display.get();

            Vec2  parentSize     = Vec2::sZero();
            float parentFont     = 0.0f;
            bool  bParentLaidOut = false;
            if (hasParent() && !isRoot())
            {
                parentSize     = m_parent->getInnerLayoutSize();
                parentFont     = m_parent->getStyle().font.size.get();
                bParentLaidOut = m_parent->hasFlag(ComponentDirty::LaidOut);
            }

            if (parentSize.x != m_layoutParentSize.x || parentSize.y != m_layoutParentSize.y)
            {
                m_layoutParentSize = parentSize;

                if (bParentLaidOut || isRoot())
                {
                    markStyleDirty();
                    markLayoutDirty();
                }
            }

            if (parentFont != m_layoutParentFontSize)
            {
                m_layoutParentFontSize = parentFont;

                if (bParentLaidOut || isRoot())
                {
                    markStyleDirty();
                    markLayoutDirty();
                }
            }

            if (hideIfDirective())
            {
                return;
            }

            if (!hasFlag(ComponentDirty::Layout) && (isWidthAuto(style) || isHeightAuto(style)))
            {
                for (Component* child : m_children)
                {
                    if (child && child->hasFlag(ComponentDirty::Layout))
                    {
                        markLayoutDirty();

                        break;
                    }
                }
            }

            if (hasFlag(ComponentDirty::LiveBind) && !m_live.isEmpty())
            {
                String resolved;
                for (const String& part : m_live.split('\n'))
                {
                    if (part.isEmpty())
                    {
                        continue;
                    }

                    resolved.append(parseText(part.contains("ref(") ? expandStyleBinding(part) : part));
                    resolved.append('\n');
                }

                std::uint64_t hash = 14695981039346656037ull;
                for (std::size_t i = 0; i < resolved.size(); i++)
                {
                    hash ^= static_cast<std::uint64_t>(static_cast<unsigned char>(resolved.at(i)));
                    hash *= 1099511628211ull;
                }

                if (hash != m_liveHash)
                {
                    m_liveHash = hash;
                    markStyleDirty();
                    markLayoutDirty();
                }
            }

            if (hasFlag(ComponentDirty::LiveBind))
            {
                refreshClassName();
            }

            const bool bStyleRefreshed = hasFlag(ComponentDirty::Style);

            if (bStyleRefreshed)
            {
                const LayoutMetrics before = LayoutMetrics::sCapture(style);

                refreshStyleRuleset();
                setFlag(ComponentDirty::Style, false);

                refreshStyle();
                setFlag(ComponentDirty::Text);

                if (!(before == LayoutMetrics::sCapture(style)))
                {
                    markLayoutDirty();
                }
            }

            if (!bStyleRefreshed)
            {
                bool bAnimatedDisplay = false;

                if (m_animator.hasPlayer())
                {
                    for (const StyleAnimatorTrack& track : m_animator.getPlayerTracks())
                    {
                        if (track.id == StylePropertyId::Display)
                        {
                            bAnimatedDisplay = true;

                            break;
                        }
                    }
                }

                if (!bAnimatedDisplay)
                {
                    const String& displayRaw = style.display.getRaw();
                    if (isReference(displayRaw) || displayRaw.contains("ref("))
                    {
                        const StyleDisplay previousDisplay = style.display.get();

                        style.display.refresh();

                        if (previousDisplay != style.display.get())
                        {
                            markLayoutDirty();
                        }
                    }
                }
            }

            refreshDirectives();
            refreshId();

            if (displayBefore == StyleDisplay::None && !style.isDisplay(StyleDisplay::None))
            {
                replayAnimations();
            }

            const bool bIsAnimating  = !m_animator.isIdle();
            const bool bIsAbsolute   = style.isPosition(StylePosition::Absolute);
            const bool bIsFlowLocked = hasParent() && !isRoot() && !bParentLaidOut && !bIsAbsolute;
            const bool bHadTween     = hasLayoutTween(m_animator);

            if (bIsAnimating || bStyleRefreshed || !m_bIsAnimationReady || m_bIsReplayingAnimation)
            {
                tickAnimation(style, m_animationDelta);

                const StylePropertyDirty dirty = m_animator.getDirty();

                const bool bHasSubtree = static_cast<bool>(has(dirty, StylePropertyDirty::Subtree));

                if (bHasSubtree)
                {
                    markPaintDirtySubtree();
                }

                const bool bHasPaint = !bHasSubtree && (has(dirty, StylePropertyDirty::Paint));

                if (bHasPaint)
                {
                    markPaintDirty();
                }

                if (has(dirty, StylePropertyDirty::Text))
                {
                    setFlag(ComponentDirty::Text);
                }
            }

            if (style.isDisplay(StyleDisplay::None))
            {
                setCulled(false);
                setFlag(ComponentDirty::Layout, false);

                return;
            }

            if (bHadTween || hasLayoutTween(m_animator))
            {
                markLayoutDirty();
            }

            const auto propagateLayout = [this](const Vec2& inSizeBefore, const Vec2& inContentBefore)
            {
                const Vec2 content = getContentSize();
                const bool bChanged =
                    std::abs(inSizeBefore.x - m_size.x) > 0.01f || std::abs(inSizeBefore.y - m_size.y) > 0.01f ||
                    std::abs(inContentBefore.x - content.x) > 0.01f || std::abs(inContentBefore.y - content.y) > 0.01f;

                if (!bChanged)
                {
                    return;
                }

                for (Component* child : m_children)
                {
                    if (child)
                    {
                        child->markLayoutDirty();
                    }
                }
            };

            const bool bHasFlagAndFlowLocked = static_cast<bool>(hasFlag(ComponentDirty::Layout) && bIsFlowLocked);

            if (bHasFlagAndFlowLocked)
            {
                const Vec2 previousSize    = m_size;
                const Vec2 previousContent = getContentSize();

                refreshSize();
                resetFlowCursor();
                refreshBounds();
                markPaintDirty();

                setFlag(ComponentDirty::Layout, false);
                setFlag(ComponentDirty::LaidOut);
                propagateLayout(previousSize, previousContent);

                if (m_parent && (std::abs(previousContent.x - getContentSize().x) > 0.01f ||
                                 std::abs(previousContent.y - getContentSize().y) > 0.01f))
                {
                    m_parent->markLayoutDirty();
                }
            }

            const bool bHasFlag = !bHasFlagAndFlowLocked && (hasFlag(ComponentDirty::Layout));

            if (bHasFlag)
            {
                const Vec2 previousSize    = m_size;
                const Vec2 previousContent = getContentSize();

                refreshSize();
                refreshPosition();
                refreshBounds();
                markPaintDirty();
                setFlag(ComponentDirty::Layout, false);
                setFlag(ComponentDirty::LaidOut);
                propagateLayout(previousSize, previousContent);
            }

            if (hasFlag(ComponentDirty::LaidOut))
            {
                if (m_animator.isIdle() && style.transform.getRaw().contains('%'))
                {
                    style.transform.refresh();
                    markPaintDirty();
                }

                applyLaidOutRadius(style, m_size);
            }

            onRefresh();
            setFlag(ComponentDirty::Text, false);
        }

        void Component::replayAnimations()
        {
            replayAnimation();

            for (Component* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                child->replayAnimations();
            }
        }

        Vec2 Component::getChildrenContentSizeBlock() const
        {
            float width  = 0.0f;
            float height = 0.0f;

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style      = child->getStyle();
                const Vec2   size       = getChildIntrinsicSize(child);
                const float  marginLeft = style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get();
                const float  marginRight =
                    style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get();
                const float marginTop = style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get();
                const float marginBottom =
                    style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get();

                height += marginTop + size.y + marginBottom;
                width = std::max(width, marginLeft + size.x + marginRight);
            }

            return Vec2(width, height);
        }

        Vec2 Component::getChildrenContentSizeFlex() const
        {
            const bool  bIsRow   = style.flex.direction.get() == StyleFlexDirection::Row;
            const bool  bCanWrap = style.flex.wrap.get() == StyleFlexWrap::Wrap;
            const float mainGap  = bIsRow ? style.gap.left.get() : style.gap.top.get();
            const float crossGap = bIsRow ? style.gap.top.get() : style.gap.left.get();
            const float wrapMain = bCanWrap ? flexWrapMainSize(this, bIsRow) : 0.0f;

            float lineMain        = 0.0f;
            float lineCross       = 0.0f;
            float totalMain       = 0.0f;
            float totalCross      = 0.0f;
            bool  bHasLineStarted = false;

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style = child->getStyle();
                Vec2         size  = getChildIntrinsicSize(child);

                if (wrapMain > 0.0f)
                {
                    const bool bRow = static_cast<bool>(bIsRow);

                    if (bRow)
                    {
                        const float percent =
                            percentOfBasis(style.width.value.getRaw(), wrapMain, SizeDirection::Horizontal);
                        if (percent >= 0.0f)
                        {
                            size.x = percent;
                        }
                    }

                    if (!bRow)
                    {
                        const float percent =
                            percentOfBasis(style.height.value.getRaw(), wrapMain, SizeDirection::Vertical);
                        if (percent >= 0.0f)
                        {
                            size.y = percent;
                        }
                    }
                }

                const float marginLeft = style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get();
                const float marginRight =
                    style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get();
                const float marginTop = style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get();
                const float marginBottom =
                    style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get();

                const float itemMain =
                    bIsRow ? (marginLeft + size.x + marginRight) : (marginTop + size.y + marginBottom);
                const float itemCross =
                    bIsRow ? (marginTop + size.y + marginBottom) : (marginLeft + size.x + marginRight);

                if (bCanWrap && bHasLineStarted && wrapMain > 0.0f && (lineMain + mainGap + itemMain) > wrapMain)
                {
                    totalMain       = std::max(totalMain, lineMain);
                    totalCross      = totalCross > 0.0f ? totalCross + crossGap + lineCross : lineCross;
                    lineMain        = 0.0f;
                    lineCross       = 0.0f;
                    bHasLineStarted = false;
                }

                if (bHasLineStarted)
                {
                    lineMain += mainGap;
                }

                lineMain += itemMain;
                lineCross       = std::max(lineCross, itemCross);
                bHasLineStarted = true;
            }

            totalMain = std::max(totalMain, lineMain);
            if (bHasLineStarted)
            {
                totalCross = totalCross > 0.0f ? totalCross + crossGap + lineCross : lineCross;
            }

            return bIsRow ? Vec2(totalMain, totalCross) : Vec2(totalCross, totalMain);
        }

        Vec2 Component::getChildrenContentSizeFromOrigin(float inOriginX, float inOriginY) const
        {
            Vec2 result = Vec2::sZero();

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style = child->getStyle();
                const Vec2   size  = getChildIntrinsicSize(child);

                const float marginRight =
                    style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get();
                const float marginBottom =
                    style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get();

                result.x = std::max(result.x, (child->getPosition().x - inOriginX) + size.x + marginRight);
                result.y = std::max(result.y, (child->getPosition().y - inOriginY) + size.y + marginBottom);
            }

            return result;
        }

        Vec2 Component::getChildrenContentSize() const
        {
            if (style.display.get() == StyleDisplay::Flex)
            {
                return getChildrenContentSizeFlex();
            }

            return getChildrenContentSizeBlock();
        }

        Vec2 Component::getChildIntrinsicSize(const Component* inChild) const
        {
            const Style& style = inChild->getStyle();
            Vec2         size  = inChild->getContentSize();

            const Style& parentStyle        = getStyle();
            const bool   bIsWidthAuto       = isWidthAuto(style);
            const bool   bIsHeightAuto      = isHeightAuto(style);
            const bool   bIsWidthIntrinsic  = isWidthIntrinsicAuto(style, parentStyle);
            const bool   bIsHeightIntrinsic = isHeightIntrinsicAuto(style, parentStyle);

            if (bIsWidthIntrinsic || bIsHeightIntrinsic)
            {
                const Vec2 inner = inChild->getChildrenContentSize();

                if (bIsWidthIntrinsic)
                {
                    size.x = inChild->hasChildren() ? inner.x : std::max(size.x, inner.x);
                }

                if (bIsHeightIntrinsic)
                {
                    size.y = std::max(size.y, inner.y);
                }
            }

            size.x += style.insetHorizontal();
            size.y += style.insetVertical();

            return size;
        }

        float Component::getDepth() const
        {
            return m_draw.depth;
        }

        const Vec2& Component::getSize() const
        {
            return m_size;
        }

        Vec2 Component::getContentSize() const
        {
            return Vec2(
                std::max(0.0f, m_size.x - style.insetHorizontal()),
                std::max(0.0f, m_size.y - style.insetVertical())
            );
        }

        Vec2 Component::getInnerLayoutSize() const
        {
            const Vec2 content = getContentSize();
            const Vec2 gutter  = getScrollBarGutter();

            return Vec2(std::max(0.0f, content.x - gutter.x), std::max(0.0f, content.y - gutter.y));
        }

        Vec2 Component::getBorderSize() const
        {
            return m_size;
        }

        Vec2 Component::getRemainingContentSize() const
        {
            return getRemainingContentSize(nullptr);
        }

        Vec2 Component::getRemainingContentSize(const Component* inChild) const
        {
            const Vec2 inner = getInnerLayoutSize();

            if (!inChild || !isFlexNowrap(style))
            {
                return inner;
            }

            const bool  bIsRow    = style.flex.direction.get() == StyleFlexDirection::Row;
            const float mainGap   = bIsRow ? style.gap.left.get() : style.gap.top.get();
            const float innerMain = bIsRow ? inner.x : inner.y;

            float used      = 0.0f;
            int   fillCount = 0;
            int   flowCount = 0;

            for (const Component* sibling : m_children)
            {
                if (!sibling || !sibling->isDisplayable() || sibling->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                if (flowCount > 0)
                {
                    used += mainGap;
                }

                ++flowCount;

                if (isMainFillPercent(sibling, bIsRow))
                {
                    ++fillCount;

                    continue;
                }

                const Style& style = sibling->getStyle();
                const Vec2   box   = getChildIntrinsicSize(sibling);
                const float  marginStart =
                    bIsRow ? (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get())
                           : (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get());
                const float marginEnd =
                    bIsRow ? (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get())
                           : (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get());

                used += marginStart + (bIsRow ? box.x : box.y) + marginEnd;
            }

            const float remainingMain = std::max(0.0f, innerMain - used);
            const float share         = fillCount > 0 && isMainFillPercent(inChild, bIsRow)
                                            ? remainingMain / static_cast<float>(fillCount)
                                            : remainingMain;

            return bIsRow ? Vec2(share, inner.y) : Vec2(inner.x, share);
        }

        float Component::getTrailingMainSize(const Component* inChild) const
        {
            if (!inChild || !isFlexNowrap(style))
            {
                return 0.0f;
            }

            const bool  bIsRow  = style.flex.direction.get() == StyleFlexDirection::Row;
            const float mainGap = bIsRow ? style.gap.left.get() : style.gap.top.get();

            float used   = 0.0f;
            bool  bAfter = false;

            for (const Component* sibling : m_children)
            {
                if (!sibling || !sibling->isDisplayable() || sibling->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                if (!bAfter)
                {
                    if (sibling == inChild)
                    {
                        bAfter = true;
                    }

                    continue;
                }

                const Style& style = sibling->getStyle();
                const Vec2   box   = getChildIntrinsicSize(sibling);
                const float  marginStart =
                    bIsRow ? (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get())
                           : (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get());
                const float marginEnd =
                    bIsRow ? (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get())
                           : (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get());
                const float itemMain = marginStart + (bIsRow ? box.x : box.y) + marginEnd;

                used += mainGap + itemMain;
            }

            return used;
        }

        int Component::countTrailingMainAutoMargins(const Component* inChild) const
        {
            if (!inChild || !isFlexNowrap(style))
            {
                return 0;
            }

            const bool bIsRow = style.flex.direction.get() == StyleFlexDirection::Row;
            int        count  = 0;
            bool       bAfter = false;

            for (const Component* sibling : m_children)
            {
                if (!sibling || !sibling->isDisplayable() || sibling->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                if (!bAfter)
                {
                    const bool bSiblingMatches = static_cast<bool>(sibling == inChild);

                    if (bSiblingMatches)
                    {
                        bAfter = true;
                    }

                    if (!bSiblingMatches)
                    {
                        continue;
                    }
                }

                const Style& style = sibling->getStyle();
                const bool   bRow  = static_cast<bool>(bIsRow);

                if (bRow)
                {
                    if (style.margin.left.isRaw(Size::AUTO_KEYWORD))
                    {
                        ++count;
                    }

                    if (style.margin.right.isRaw(Size::AUTO_KEYWORD))
                    {
                        ++count;
                    }
                }

                if (!bRow)
                {
                    if (style.margin.top.isRaw(Size::AUTO_KEYWORD))
                    {
                        ++count;
                    }

                    if (style.margin.bottom.isRaw(Size::AUTO_KEYWORD))
                    {
                        ++count;
                    }
                }
            }

            return count;
        }

        void Component::addSize(const Vec2& inValue)
        {
            addSize(inValue.x, inValue.y);
        }

        void Component::addSize(float inWidth, float inHeight)
        {
            if (inWidth == 0.0f && inHeight == 0.0f)
            {
                return;
            }

            float width  = m_size.x + inWidth;
            float height = m_size.y + inHeight;
            style.clampSize(width, height);
            m_size.x = width;
            m_size.y = height;
            markPaintDirtySubtree();
        }

        void Component::setSize(const Vec2& inValue)
        {
            setSize(inValue.x, inValue.y);
        }

        void Component::setSize(float inWidth, float inHeight)
        {
            float width  = inWidth;
            float height = inHeight;
            style.clampSize(width, height);

            if (m_size.x == width && m_size.y == height)
            {
                return;
            }

            m_size.x = width;
            m_size.y = height;
            markPaintDirty();
            markLayoutDirty();
        }

        const Vec2& Component::getScale() const
        {
            return (m_scale.x <= 0.0f && m_scale.y <= 0.0f) ? m_size : m_scale;
        }

        void Component::setScale(const Vec2& inValue)
        {
            setScale(inValue.x, inValue.y);
        }

        void Component::setScale(float inX, float inY)
        {
            m_scale.x = inX;
            m_scale.y = inY;
        }

        const Vec2& Component::getOffset() const
        {
            return m_offset;
        }

        void Component::setOffset(const Vec2& inValue)
        {
            setOffset(inValue.x, inValue.y);
        }

        void Component::setOffset(float inX, float inY)
        {
            m_offset.x = inX;
            m_offset.y = inY;
        }

        Vec2 Component::getVisualCenter() const
        {
            const Vec2& size   = getSize();
            const Vec2& offset = getOffset();

            return getDrawPosition() + Vec2(size.x * 0.5f + offset.x, size.y * 0.5f - offset.y);
        }

        const Vec2& Component::getPosition() const
        {
            return getTranslation();
        }

        void Component::addPosition(const Vec2& inValue)
        {
            addPosition(inValue.x, inValue.y);
        }

        void Component::addPosition(float inX, float inY)
        {
            addAbsoluteTranslation(inX, inY);
            setCursor(getTranslation());
            m_scratch = 0.0f;
        }

        void Component::setPosition(const Vec2& inValue)
        {
            setPosition(inValue.x, inValue.y);
        }

        void Component::setPosition(float inX, float inY)
        {
            setAbsoluteTranslation(inX, inY);
            setCursor(getTranslation());
            m_scratch = 0.0f;
        }

        const Vec2& Component::getCursor() const
        {
            return m_cursor;
        }

        void Component::addCursor(const Vec2& inValue)
        {
            addCursor(inValue.x, inValue.y);
        }

        void Component::addCursor(float inX, float inY)
        {
            setCursor(m_cursor.x + inX, m_cursor.y + inY);
        }

        void Component::setCursor(const Vec2& inValue)
        {
            setCursor(inValue.x, inValue.y);
        }

        void Component::setCursor(float inX, float inY)
        {
            m_cursor.x = inX;
            m_cursor.y = inY;
        }

        void Component::refreshBounds()
        {
            m_bounds.set(0.0f, 0.0f, m_size.y, m_size.x);
            m_bounds.transform(getMatrix());
        }

        void Component::resetFlowCursor()
        {
            setCursor(getPosition().x + style.insetLeft(), getPosition().y + style.insetTop());
            m_scratch = 0.0f;
        }

        void Component::reflowChildPositions()
        {
            resetFlowCursor();

            for (Component* child : m_children)
            {
                if (!child || child->style.isDisplay(StyleDisplay::None))
                {
                    continue;
                }

                child->refreshPosition();
                child->refreshBounds();
                child->reflowChildPositions();
            }
        }
    }
}
