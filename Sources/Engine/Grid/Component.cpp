#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Time.hpp"

#include <glm/gtc/matrix_inverse.hpp>

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        thread_local Component*               g_scope     = nullptr;
        thread_local std::vector<Component*>* g_projected = nullptr;

        struct Scope
        {
        public:
            explicit Scope(Component* inComponent)
                : previous(g_scope)
            {
                g_scope = inComponent;
            }

            ~Scope() { g_scope = previous; }

        public:
            Component* previous = nullptr;
        };

        struct LayoutMetrics
        {
            StyleDisplay       display;
            StylePosition      position;
            StyleAlignment     align;
            StyleFlexDirection flexDir;
            StyleFlexWrap      flexWrap;
            StyleWordBreak     wordBreak;
            String             widthRaw;
            String             heightRaw;
            String             minWidthRaw;
            String             minHeightRaw;
            String             maxWidthRaw;
            String             maxHeightRaw;
            String             marginL;
            String             marginR;
            String             marginT;
            String             marginB;
            String             paddingL;
            String             paddingR;
            String             paddingT;
            String             paddingB;
        };

        bool operator==(const LayoutMetrics& inLeft, const LayoutMetrics& inRight)
        {
            return inLeft.display == inRight.display && inLeft.position == inRight.position &&
                   inLeft.align == inRight.align && inLeft.flexDir == inRight.flexDir &&
                   inLeft.flexWrap == inRight.flexWrap && inLeft.wordBreak == inRight.wordBreak &&
                   inLeft.widthRaw.equals(inRight.widthRaw) && inLeft.heightRaw.equals(inRight.heightRaw) &&
                   inLeft.minWidthRaw.equals(inRight.minWidthRaw) && inLeft.minHeightRaw.equals(inRight.minHeightRaw) &&
                   inLeft.maxWidthRaw.equals(inRight.maxWidthRaw) && inLeft.maxHeightRaw.equals(inRight.maxHeightRaw) &&
                   inLeft.marginL.equals(inRight.marginL) && inLeft.marginR.equals(inRight.marginR) &&
                   inLeft.marginT.equals(inRight.marginT) && inLeft.marginB.equals(inRight.marginB) &&
                   inLeft.paddingL.equals(inRight.paddingL) && inLeft.paddingR.equals(inRight.paddingR) &&
                   inLeft.paddingT.equals(inRight.paddingT) && inLeft.paddingB.equals(inRight.paddingB);
        }

        static LayoutMetrics captureLayoutMetrics(const Style& inStyle)
        {
            return {inStyle.display.get(),          inStyle.position.get(),         inStyle.align.get(),
                    inStyle.flex.direction.get(),   inStyle.flex.wrap.get(),        inStyle.wordBreak.get(),
                    inStyle.width.value.getRaw(),   inStyle.height.value.getRaw(),  inStyle.width.min.getRaw(),
                    inStyle.height.min.getRaw(),    inStyle.width.max.getRaw(),     inStyle.height.max.getRaw(),
                    inStyle.margin.left.getRaw(),   inStyle.margin.right.getRaw(),  inStyle.margin.top.getRaw(),
                    inStyle.margin.bottom.getRaw(), inStyle.padding.left.getRaw(),  inStyle.padding.right.getRaw(),
                    inStyle.padding.top.getRaw(),   inStyle.padding.bottom.getRaw()};
        }

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
                return Vec2::Zero();
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

            if (bIsRow)
            {
                if (isWidthAuto(style) && !stretchesAutoWidth(inBox))
                {
                    return 0.0f;
                }

                if (isWidthAuto(style) || style.isFillPercent(style.width.value.getRaw()))
                {
                    basis -= (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get());
                    basis -= (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get());
                }
                else
                {
                    const float parsed = percentOfBasis(style.width.value.getRaw(), basis, SizeDirection::Horizontal);
                    if (parsed >= 0.0f)
                    {
                        basis = parsed;
                    }
                }

                basis -= style.insetHorizontal();
            }
            else
            {
                if (isHeightAuto(style))
                {
                    return 0.0f;
                }

                if (style.isFillPercent(style.height.value.getRaw()))
                {
                    basis -= (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get());
                    basis -= (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get());
                }
                else
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

        struct Projection
        {
        public:
            explicit Projection(std::vector<Component*>& inChildren)
                : previous(g_projected)
            {
                g_projected = &inChildren;
            }

            ~Projection() { g_projected = previous; }

        public:
            std::vector<Component*>* previous = nullptr;
        };

        struct Loading
        {
        public:
            Loading(std::vector<String>& inStack, const String& inPath)
                : stack(inStack)
            {
                stack.push_back(inPath);
            }

            ~Loading() { stack.pop_back(); }

        public:
            std::vector<String>& stack;
        };

        Component* Component::create(const XmlNode& inNode)
        {
            if (inNode.empty() || !inNode.isElement() || isContentSlot(inNode))
            {
                return nullptr;
            }

            const String              tag = inNode.getName();
            const ReflectionTypeInfo* type =
                ReflectionTypeRegistry::getInstance().find(String("Chicane::Grid::") + tag);

            if (!type)
            {
                type = ReflectionTypeRegistry::getInstance().find(tag);
            }

            if (!type && !tag.isEmpty())
            {
                const String pascal = tag.substr(0, 1).toUpper() + tag.substr(1);

                if (!pascal.equals(tag))
                {
                    type = ReflectionTypeRegistry::getInstance().find(String("Chicane::Grid::") + pascal);
                }
            }

            if (!type && g_scope)
            {
                type = g_scope->findImported(tag);
            }

            if (!type)
            {
                return nullptr;
            }

            Component* instance = type->create<Component>({inNode});
            if (instance)
            {
                instance->m_importOwner = g_scope;
            }

            return instance;
        }

        bool Component::isContentSlot(const XmlNode& inNode)
        {
            if (inNode.empty() || !inNode.isElement())
            {
                return false;
            }

            return String(inNode.getName()).equals(CONTENT_TAG_ID);
        }

        Component::Component(const XmlNode& inNode)
            : Component(inNode.getName())
        {
            parse(inNode);
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            addChildren(getSource());

            setDirective(
                IF_DIRECTIVE_KEYWORD,
                [&](const String& inValue)
                {
                    if (inValue.isEmpty())
                    {
                        return;
                    }

                    if (parseText(inValue).equals("true", "1"))
                    {
                        if (m_style.display.getRaw().isEmpty())
                        {
                            m_style.display.set(StyleDisplay::Block);
                        }
                        else
                        {
                            m_style.display.refresh();
                        }

                        return;
                    }

                    m_style.display.set(StyleDisplay::None);
                }
            );

            setDirective(
                FOR_DIRECTIVE_KEYWORD,
                [&](const String& inValue)
                {
                    if (inValue.isEmpty())
                    {
                        return;
                    }

                    const std::vector<String> values = inValue.trim().split(':');
                    if (values.size() < 2)
                    {
                        return;
                    }

                    const String variableId = values.at(0).trim();
                    const String accessorId = values.at(1).trim();

                    Component* owner = hasParent() ? getParent() : this;
                    while (owner)
                    {
                        ReflectionFieldAccessor accessor = owner->getField(accessorId);

                        if (accessor.isValid() && accessor.bIsIterable)
                        {
                            m_forVariable = variableId;

                            if (accessor.iterable.snapshotFunction)
                            {
                                const void* container = accessor.address(owner);
                                const void* frozen =
                                    container ? accessor.iterable.snapshotFunction(container, m_forSource) : nullptr;

                                if (frozen)
                                {
                                    ReflectionFieldAccessor frozenAccessor = accessor;
                                    frozenAccessor.offset                  = 0;
                                    frozenAccessor.ptrOffset               = 0;
                                    frozenAccessor.bNeedsDeref             = false;
                                    frozenAccessor.boundInstance           = frozen;

                                    syncForLoop(variableId, frozenAccessor, nullptr);

                                    return;
                                }
                            }

                            m_forSource = {};

                            syncForLoop(variableId, accessor, owner);

                            return;
                        }

                        if (!owner->hasParent())
                        {
                            break;
                        }

                        owner = owner->getParent();
                    }

                    if (!isMethod(accessorId) && !isReference(accessorId))
                    {
                        return;
                    }

                    ReflectionTypeMethod            method     = getMethod(accessorId);
                    const ReflectionTypeMethodInfo* methodInfo = method.getInfo();

                    if (!method.isValid() || !methodInfo || !methodInfo->isIterable())
                    {
                        return;
                    }

                    m_forSource                      = method.invoke();
                    ReflectionFieldAccessor accessor = methodInfo->makeAccessor(m_forSource);

                    if (!accessor.isValid())
                    {
                        m_forSource = {};

                        return;
                    }

                    m_forVariable = variableId;
                    syncForLoop(variableId, accessor, nullptr);
                }
            );
        }

        Component::Component(const String& inTag)
            : Animatable(),
              m_tag(inTag),
              m_id(String::empty()),
              m_className(String::empty()),
              m_status(ComponentStatus::None),
              m_flags(ComponentDirty::Style | ComponentDirty::Layout),
              m_live(String::empty()),
              m_liveHash(0),
              m_directives({}),
              m_variables({}),
              m_style({}),
              m_styleVariables({}),
              m_styleFile(nullptr),
              m_styles(nullptr),
              m_bHasOwnStyle(false),
              m_imports({}),
              m_importOwner(nullptr),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_peripherals({}),
              m_paintChildren({}),
              m_size(Vec2::Zero()),
              m_scale(Vec2::Zero()),
              m_offset(Vec2::Zero()),
              m_cursor(Vec2::Zero()),
              m_scratch(0.0f),
              m_layoutParentSize(Vec2(-1.0f)),
              m_layoutParentFontSize(-1.0f),
              m_primitive({}),
              m_draw({}),
              m_forInstances({}),
              m_forVariable(String::empty()),
              m_forSource({})
        {
            m_style.setParent(this);
        }

        Component::~Component()
        {
            if (View* view = dynamic_cast<View*>(m_root))
            {
                view->clearInteraction(this);
            }

            for (Component* child : m_children)
            {
                delete child;
                child = nullptr;
            }

            m_children.clear();
        }

        bool Component::canPlayAnimation() const
        {
            return isDisplayable();
        }

        bool Component::isDrawable() const
        {
            return isDisplayable() && isVisible() && isSolid() && hasPrimitive() && !isCulled();
        }

        bool Component::isFocusable() const
        {
            return false;
        }

        bool Component::escapesOverflow() const
        {
            return false;
        }

        bool Component::onEvent(const WindowEvent&)
        {
            return false;
        }

        void Component::refreshStyleRuleset()
        {
            if (m_bIsAnimationReady)
            {
                m_style.snapshot();
            }

            m_styleVariables.clear();

            m_style.resetValues();

            std::vector<StyleFile*> files;
            auto                    addFile = [&](StyleFile* file)
            {
                if (!file)
                {
                    return;
                }

                if (std::find(files.begin(), files.end(), file) != files.end())
                {
                    return;
                }

                files.push_back(file);
            };

            std::vector<StyleFile*> ancestors;
            for (Component* ancestor = m_parent; ancestor && ancestor != this;)
            {
                if (ancestor->m_styleFile)
                {
                    ancestors.push_back(ancestor->m_styleFile);
                }

                if (ancestor->m_bHasOwnStyle)
                {
                    ancestors.push_back(ancestor->m_styles.get());
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            for (auto it = ancestors.rbegin(); it != ancestors.rend(); it++)
            {
                addFile(*it);
            }

            addFile(m_styleFile);
            if (m_bHasOwnStyle)
            {
                addFile(m_styles.get());
            }

            if (files.empty())
            {
                m_style.restore();

                return;
            }

            struct StyleMatch
            {
                std::uint32_t       origin      = 0;
                std::uint32_t       specificity = 0;
                std::uint32_t       order       = 0;
                const StyleRuleset* source      = nullptr;
            };

            std::vector<StyleMatch> matches;
            std::uint32_t           order = 0;

            for (StyleFile* file : files)
            {
                const std::uint32_t origin =
                    (file == m_styleFile || (m_bHasOwnStyle && file == m_styles.get())) ? 1U : 0U;

                for (const StyleRuleset& source : file->getRulesets())
                {
                    if (source.isEmpty())
                    {
                        continue;
                    }

                    bool          bHasMatched = false;
                    std::uint32_t specificity = 0;

                    if (!source.compiled.empty())
                    {
                        for (const StyleCompiledSelector& selector : source.compiled)
                        {
                            if (!matchesCompiledSelector(selector))
                            {
                                continue;
                            }

                            bHasMatched = true;
                            specificity = std::max(specificity, selector.specificity());
                        }
                    }
                    else
                    {
                        for (const String& selector : source.selectors)
                        {
                            if (!hasSelector(selector.trim()))
                            {
                                continue;
                            }

                            bHasMatched = true;
                        }
                    }

                    if (!bHasMatched)
                    {
                        continue;
                    }

                    matches.push_back({origin, specificity, order++, &source});
                }
            }

            std::stable_sort(
                matches.begin(),
                matches.end(),
                [](const StyleMatch& inLeft, const StyleMatch& inRight)
                {
                    if (inLeft.specificity != inRight.specificity)
                    {
                        return inLeft.specificity < inRight.specificity;
                    }

                    if (inLeft.origin != inRight.origin)
                    {
                        return inLeft.origin < inRight.origin;
                    }

                    return inLeft.order < inRight.order;
                }
            );

            StyleRuleset::Properties properties;
            for (const StyleMatch& match : matches)
            {
                for (const auto& [key, value] : match.source->properties)
                {
                    if (key.startsWith(Style::VARIABLE_KEYWORD))
                    {
                        m_styleVariables[key.substr(1)] = value;

                        continue;
                    }

                    properties[key] = value;
                }
            }

            if (!properties.empty())
            {
                addStyleProperties(properties);
            }

            if (hasParent() && !isRoot() && properties.find(Style::FOREGROUND_COLOR_ATTRIBUTE_NAME) == properties.end())
            {
                m_style.foregroundColor.copyValue(m_parent->getStyle().foregroundColor);
            }

            m_live = String::empty();
            for (const auto& [key, value] : properties)
            {
                if (value.contains("ref(") || isReference(value))
                {
                    if (!m_live.isEmpty())
                    {
                        m_live.append('\n');
                    }

                    m_live.append(value);
                }
            }

            setFlag(ComponentDirty::LiveBind, isReference(getAttribute(CLASS_ATTRIBUTE_NAME)) || !m_live.isEmpty());

            m_style.restore();
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            if (hasParent() && m_parent->hasFlag(ComponentDirty::LaidOut) &&
                !m_style.isPosition(StylePosition::Absolute) && isFlexNowrap(m_parent->getStyle()) &&
                (m_style.isFillPercent(m_style.width.value.getRaw()) ||
                 m_style.isFillPercent(m_style.height.value.getRaw())))
            {
                m_style.resolveFillPercent(m_parent->getRemainingContentSize(this));
            }

            const bool bIsWidthAuto  = m_style.width.isAuto();
            const bool bIsHeightAuto = m_style.height.isAuto();

            float width  = m_style.width.value.get();
            float height = m_style.height.value.get();

            const bool bStretchWidth = bIsWidthAuto && stretchesAutoWidth(this);
            if (bStretchWidth)
            {
                const Component* box       = getContainingBlock();
                const float      available = innerLayoutSize(box).x;
                const float      horizontalMargin =
                    (m_style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.left.get()) +
                    (m_style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.right.get());

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
                    width = content.x + m_style.insetHorizontal();
                }

                if (bIsHeightAuto)
                {
                    height = content.y + m_style.insetVertical();
                }
            }

            m_style.clampSize(width, height);
            setSize(width, height);
            setFlag(ComponentDirty::Insets);
        }

        void Component::refreshPosition()
        {
            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            setPosition(0.0f, 0.0f);

            float marginLeft   = m_style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.left.get();
            float marginRight  = m_style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.right.get();
            float marginTop    = m_style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.top.get();
            float marginBottom = m_style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.bottom.get();

            const Vec2 startPadding(m_style.insetLeft(), m_style.insetTop());

            const Vec2 contentSize = getContentSize();

            const float usedWidth  = contentSize.x + m_style.insetHorizontal();
            const float usedHeight = contentSize.y + m_style.insetVertical();

            if (hasParent() && !isRoot())
            {
                const Component* box = m_style.isPosition(StylePosition::Absolute) ? getContainingBlock() : m_parent;
                const Style&     parentStyle = m_parent->getStyle();
                const Vec2       available   = innerLayoutSize(box);

                const bool bIsLeftAuto     = m_style.margin.left.isRaw(Size::AUTO_KEYWORD);
                const bool bIsRightAuto    = m_style.margin.right.isRaw(Size::AUTO_KEYWORD);
                const bool bIsTopAuto      = m_style.margin.top.isRaw(Size::AUTO_KEYWORD);
                const bool bIsBottomAuto   = m_style.margin.bottom.isRaw(Size::AUTO_KEYWORD);
                const bool bIsParentCenter = parentStyle.align.get() == StyleAlignment::Center;
                const bool bIsParentFlex   = parentStyle.isDisplay(StyleDisplay::Flex);
                const bool bIsParentRow = bIsParentFlex && parentStyle.flex.direction.get() == StyleFlexDirection::Row;

                float leftoverW = available.x - usedWidth - marginLeft - marginRight;

                if (!m_style.isPosition(StylePosition::Absolute) && bIsParentRow)
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
                        !m_style.isPosition(StylePosition::Absolute) && bIsParentRow && (bIsLeftAuto || bIsRightAuto);
                    const int   mainAutos = bShareMainAuto ? m_parent->countTrailingMainAutoMargins(this) : 0;
                    const float mainShare = mainAutos > 0 ? leftoverW / static_cast<float>(mainAutos) : leftoverW;

                    if (bShareMainAuto && mainAutos > 0)
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
                    else if (bIsParentCenter || (bIsLeftAuto && bIsRightAuto))
                    {
                        marginLeft  = leftoverW * 0.5f;
                        marginRight = leftoverW * 0.5f;
                    }
                    else if (bIsLeftAuto)
                    {
                        marginLeft = leftoverW;
                    }
                    else if (bIsRightAuto)
                    {
                        marginRight = leftoverW;
                    }
                }

                const bool bIsParentHeightAuto = parentStyle.height.isAuto();

                const bool bCanAutoVertical =
                    m_style.isPosition(StylePosition::Absolute) || bIsParentFlex || !bIsParentHeightAuto;

                if (bCanAutoVertical && (bIsParentCenter || bIsTopAuto || bIsBottomAuto))
                {
                    float leftoverH = available.y - usedHeight - marginTop - marginBottom;

                    if (!m_style.isPosition(StylePosition::Absolute) && bIsParentFlex && !bIsParentRow)
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
                        const bool bShareMainAuto = !m_style.isPosition(StylePosition::Absolute) && bIsParentFlex &&
                                                    !bIsParentRow && (bIsTopAuto || bIsBottomAuto);
                        const int   mainAutos = bShareMainAuto ? m_parent->countTrailingMainAutoMargins(this) : 0;
                        const float mainShare = mainAutos > 0 ? leftoverH / static_cast<float>(mainAutos) : leftoverH;

                        if (bShareMainAuto && mainAutos > 0)
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
                        else if (bIsParentCenter || (bIsTopAuto && bIsBottomAuto))
                        {
                            marginTop    = leftoverH * 0.5f;
                            marginBottom = leftoverH * 0.5f;
                        }
                        else if (bIsTopAuto)
                        {
                            marginTop = leftoverH;
                        }
                        else if (bIsBottomAuto)
                        {
                            marginBottom = leftoverH;
                        }
                    }
                }
            }

            if (isRoot() || m_style.isPosition(StylePosition::Absolute))
            {
                Vec2 origin = Vec2::Zero();

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
                    if (bCanWrap && (cursorMain + mainGap + itemMain) > lineLimit)
                    {
                        if (bIsRow)
                        {
                            m_parent->setCursor(lineStart, m_parent->getCursor().y + m_parent->m_scratch + crossGap);
                        }
                        else
                        {
                            m_parent->setCursor(m_parent->getCursor().x + m_parent->m_scratch + crossGap, lineStart);
                        }

                        m_parent->m_scratch = 0.0f;
                    }
                    else if (bIsRow)
                    {
                        m_parent->addCursor(mainGap, 0.0f);
                    }
                    else
                    {
                        m_parent->addCursor(0.0f, mainGap);
                    }
                }

                setPosition(m_parent->getCursor() + Vec2(marginLeft, marginTop));
                addCursor(startPadding);

                m_parent->m_scratch = std::max(m_parent->m_scratch, itemCross);

                if (bIsRow)
                {
                    m_parent->addCursor(marginLeft + usedWidth + marginRight, 0.0f);
                }
                else
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

        bool Component::isRoot() const
        {
            return (!m_parent && !m_root) || (m_parent == this && m_root == this);
        }

        bool Component::isDisplayable() const
        {
            if (!isRoot())
            {
                if (!m_parent || !m_parent->isDisplayable())
                {
                    return false;
                }
            }

            return !m_style.isDisplay(StyleDisplay::None) && !m_style.isDisplay(StyleDisplay::Hidden);
        }

        bool Component::isVisible() const
        {
            const bool bIsBackgroundImageVisible    = !m_style.background.image.getRaw().isEmpty();
            const bool bIsBackgroundColorVisible    = m_style.background.color.get().a > 0.0f;
            const bool bIsBackgroundGradientVisible = StyleGradient::isActive(m_style.background.gradients);
            const bool bIsBackdropVisible           = m_style.backdrop.blur.get() > 0.0f;
            const bool bIsBorderVisible             = m_style.border.isVisible();

            return (bIsBackgroundImageVisible || bIsBackgroundColorVisible || bIsBackgroundGradientVisible ||
                    bIsBackdropVisible || bIsBorderVisible) &&
                   getOpacity() > 0.0f;
        }

        bool Component::isSolid() const
        {
            return m_size.x > 0.0f && m_size.y > 0.0f;
        }

        bool Component::isHovered() const
        {
            return hasStatus(ComponentStatus::Hovered);
        }

        bool Component::isFocused() const
        {
            return hasStatus(ComponentStatus::Focused);
        }

        bool Component::isDragging() const
        {
            return hasStatus(ComponentStatus::Dragging);
        }

        bool Component::isCulled() const
        {
            return hasStatus(ComponentStatus::Culled);
        }

        ComponentStatus Component::getStatus() const
        {
            return m_status;
        }

        bool Component::hasStatus(ComponentStatus inStatus) const
        {
            return has(m_status, inStatus);
        }

        bool Component::hasFlag(ComponentDirty inFlag) const
        {
            return has(m_flags, inFlag);
        }

        void Component::setFlag(ComponentDirty inFlag, bool inEnabled)
        {
            if (inEnabled)
            {
                m_flags |= inFlag;
            }
            else
            {
                m_flags &= ~inFlag;
            }
        }

        bool Component::canAdopt(Component* inComponent) const
        {
            if (!inComponent || inComponent == this)
            {
                return false;
            }

            for (const Component* ancestor = this; ancestor != nullptr;
                 ancestor                  = ancestor->isRoot() ? nullptr : ancestor->m_parent)
            {
                if (ancestor == inComponent)
                {
                    return false;
                }
            }

            return true;
        }

        void Component::hover()
        {
            onHover();

            getMethod(getAttribute(ON_HOVER_ATTRIBUTE_NAME)).invoke();
        }

        void Component::leave()
        {
            onLeave();

            getMethod(getAttribute(ON_LEAVE_ATTRIBUTE_NAME)).invoke();
        }

        void Component::click()
        {
            onClick();

            getMethod(getAttribute(ON_CLICK_ATTRIBUTE_NAME)).invoke();
        }

        void Component::focus()
        {
            onFocus();

            getMethod(getAttribute(ON_FOCUS_ATTRIBUTE_NAME)).invoke();
        }

        void Component::blur()
        {
            onBlur();

            getMethod(getAttribute(ON_BLUR_ATTRIBUTE_NAME)).invoke();
        }

        void Component::drag()
        {
            onDrag();

            getMethod(getAttribute(ON_DRAG_ATTRIBUTE_NAME)).invoke();
        }

        void Component::endDrag()
        {
            onDragEnd();

            getMethod(getAttribute(ON_DRAG_END_ATTRIBUTE_NAME)).invoke();
        }

        void Component::setHovered(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isHovered() == inValue)
            {
                return;
            }

            if (inValue)
            {
                m_status |= ComponentStatus::Hovered;
            }
            else
            {
                m_status &= ~ComponentStatus::Hovered;
            }

            if (bShouldInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }
            else
            {
                markStyleDirty();
            }

            if (inValue)
            {
                hover();

                return;
            }

            leave();
        }

        void Component::setFocused(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isFocused() == inValue)
            {
                return;
            }

            if (inValue)
            {
                m_status |= ComponentStatus::Focused;
            }
            else
            {
                m_status &= ~ComponentStatus::Focused;
            }

            if (bShouldInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }
            else
            {
                markStyleDirty();
            }

            if (inValue)
            {
                focus();

                return;
            }

            blur();
        }

        void Component::setDragging(bool inValue, bool bShouldInvalidateSubtree)
        {
            if (isDragging() == inValue)
            {
                return;
            }

            if (inValue)
            {
                m_status |= ComponentStatus::Dragging;
            }
            else
            {
                m_status &= ~ComponentStatus::Dragging;
            }

            if (bShouldInvalidateSubtree)
            {
                markStyleDirtySubtree();
            }
            else
            {
                markStyleDirty();
            }

            if (inValue)
            {
                drag();

                return;
            }

            endDrag();
        }

        void Component::setCulled(bool inValue)
        {
            if (isCulled() == inValue)
            {
                return;
            }

            if (inValue)
            {
                m_status |= ComponentStatus::Culled;
            }
            else
            {
                m_status &= ~ComponentStatus::Culled;
            }
        }

        void Component::tick(float inDeltaTime)
        {
            m_animationDelta = inDeltaTime;
            setFlag(ComponentDirty::LaidOut, false);

            refresh();

            m_animationDelta = 0.0f;

            if (m_style.isDisplay(StyleDisplay::None))
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

            const bool bIsWidthAuto  = isWidthAuto(m_style);
            const bool bIsHeightAuto = isHeightAuto(m_style);

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

            if (m_style.isPosition(StylePosition::Absolute))
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
                    if (m_style.isPosition(StylePosition::Absolute))
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

            applyLaidOutRadius(m_style, m_size);
            refreshBounds();
            markPaintDirtySubtree();

            setFlag(ComponentDirty::Layout, false);
        }

        void Component::refresh()
        {
            Vec2  parentSize     = Vec2::Zero();
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

            if (bParentLaidOut)
            {
                markLayoutDirty();
            }

            if (!hasFlag(ComponentDirty::Layout) && (isWidthAuto(m_style) || isHeightAuto(m_style)))
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

            if (hasFlag(ComponentDirty::LiveBind) || hasFlag(ComponentDirty::Style))
            {
                refreshClassName();
            }

            const bool bStyleRefreshed = hasFlag(ComponentDirty::Style);

            if (bStyleRefreshed)
            {
                const LayoutMetrics before = captureLayoutMetrics(m_style);

                refreshStyleRuleset();
                setFlag(ComponentDirty::Style, false);
                refreshStyle();

                if (!(before == captureLayoutMetrics(m_style)))
                {
                    markLayoutDirty();
                }
            }
            else
            {
                const StyleDisplay previousDisplay = m_style.display.get();

                m_style.display.refresh();

                if (previousDisplay != m_style.display.get())
                {
                    markLayoutDirty();
                }
            }

            refreshDirectives();
            refreshId();

            if (m_style.isDisplay(StyleDisplay::None))
            {
                setCulled(false);
                setFlag(ComponentDirty::Layout, false);

                return;
            }

            const bool bIsAnimating  = !m_animator.isIdle();
            const bool bIsAbsolute   = m_style.isPosition(StylePosition::Absolute);
            const bool bIsFlowLocked = hasParent() && !isRoot() && !bParentLaidOut && !bIsAbsolute;
            const bool bHadTween     = hasLayoutTween(m_animator);

            if (bIsAnimating || bStyleRefreshed || !m_bIsAnimationReady)
            {
                tickAnimation(m_style, m_animationDelta);

                const StylePropertyDirty dirty = m_animator.getDirty();

                if (has(dirty, StylePropertyDirty::Subtree))
                {
                    markPaintDirtySubtree();
                }
                else if (has(dirty, StylePropertyDirty::Paint))
                {
                    markPaintDirty();
                }

                if (has(dirty, StylePropertyDirty::Text))
                {
                    setFlag(ComponentDirty::Text);
                }
            }

            if (bHadTween || hasLayoutTween(m_animator))
            {
                markLayoutDirty();
            }

            if (hasFlag(ComponentDirty::Layout) && bIsFlowLocked)
            {
                const Vec2 previousContent = getContentSize();

                refreshSize();
                resetFlowCursor();
                refreshBounds();
                markPaintDirty();

                setFlag(ComponentDirty::Layout, false);
                setFlag(ComponentDirty::LaidOut);

                for (Component* child : m_children)
                {
                    if (child)
                    {
                        child->markLayoutDirty();
                    }
                }

                if (m_parent && (std::abs(previousContent.x - getContentSize().x) > 0.01f ||
                                 std::abs(previousContent.y - getContentSize().y) > 0.01f))
                {
                    m_parent->markLayoutDirty();
                }
            }
            else if (hasFlag(ComponentDirty::Layout))
            {
                refreshSize();
                refreshPosition();
                refreshBounds();
                markPaintDirty();
                setFlag(ComponentDirty::Layout, false);
                setFlag(ComponentDirty::LaidOut);

                for (Component* child : m_children)
                {
                    if (child)
                    {
                        child->markLayoutDirty();
                    }
                }
            }

            if (hasFlag(ComponentDirty::LaidOut))
            {
                if (m_animator.isIdle() && m_style.transform.getRaw().contains('%'))
                {
                    m_style.transform.refresh();
                    markPaintDirty();
                }

                applyLaidOutRadius(m_style, m_size);
            }

            onRefresh();
        }

        const String& Component::getTag() const
        {
            return m_tag;
        }

        String Component::getId() const
        {
            return m_id;
        }

        void Component::setId(const String& inValue)
        {
            if (m_id.equals(inValue))
            {
                return;
            }

            m_id = inValue;

            markStyleDirty();
            markLayoutDirty();
        }

        Component::ClassList Component::getClassList() const
        {
            if (m_className.isEmpty())
            {
                return {};
            }

            ClassList result;

            String accumulated = "";
            for (const String& className : m_className.split(Style::SELECTOR_SEPARATOR_SPACE))
            {
                String part = Style::CLASS_SELECTOR + className;
                part        = part.trim();

                accumulated.append(' ');
                accumulated.append(part);

                result.emplace(std::move(part));
            }

            if (!accumulated.isEmpty())
            {
                result.insert(accumulated.trim());
            }

            return result;
        }

        const String& Component::getClassName() const
        {
            return m_className;
        }

        void Component::setClassName(const String& inValue)
        {
            if (m_className.equals(inValue))
            {
                return;
            }

            m_className = inValue;

            markStyleDirtySubtree();
            markLayoutDirtySubtree();
        }

        void Component::refreshDirectives()
        {
            for (const auto& [key, directive] : m_directives)
            {
                runDirective(key, getAttribute(key));
            }
        }

        void Component::runDirective(const String& inKey, const String& inValue)
        {
            const auto& found = m_directives.find(inKey);

            if (found == m_directives.end() || !found->second)
            {
                return;
            }

            found->second(inValue);
        }

        void Component::setDirective(const String& inKey, const Directive& inValue)
        {
            m_directives[inKey] = inValue;
        }

        bool Component::hasStyleFile() const
        {
            return m_styleFile != nullptr;
        }

        const StyleFile* Component::getStyleFile() const
        {
            return m_styleFile;
        }

        void Component::setStyleFile(StyleFile* inSource)
        {
            m_styleFile = inSource;

            refreshStyleRuleset();

            for (Component* child : m_children)
            {
                child->setStyleFile(child->m_bHasOwnStyle ? child->m_styleFile : inSource);
            }
        }

        void Component::importStyleFile(const FileSystem::Path& inValue)
        {
            if (inValue.isEmpty())
            {
                return;
            }

            if (!m_styles)
            {
                m_styles = std::make_unique<StyleFile>();
            }

            m_styles->parse(inValue);
            m_bHasOwnStyle = true;
            setStyleFile(m_styles.get());
        }

        void Component::addStyleRuleset(const StyleRuleset::List& inSources)
        {
            if (!hasStyleFile())
            {
                return;
            }

            m_styleFile->addRuleset(inSources);

            refreshStyleRuleset();
        }

        void Component::addStyleProperties(const StyleRuleset::Properties& inSource)
        {
            m_style.setProperties(inSource);
        }

        const Style& Component::getStyle() const
        {
            return m_style;
        }

        float Component::getOpacity() const
        {
            return m_draw.opacity;
        }

        float Component::getFilterBlur() const
        {
            return m_draw.filterBlur;
        }

        const String& Component::getStyleVariable(const String& inName) const
        {
            const auto found = m_styleVariables.find(inName);
            if (found != m_styleVariables.end())
            {
                return found->second;
            }

            if (hasStyleFile())
            {
                const String& value = m_styleFile->getVariable(inName);
                if (!value.isEmpty())
                {
                    return value;
                }
            }

            if (hasParent() && !isRoot())
            {
                return m_parent->getStyleVariable(inName);
            }

            return String::empty();
        }

        void Component::refreshStyleSubtree()
        {
            refreshStyleRuleset();

            for (Component* child : m_children)
            {
                child->refreshStyleSubtree();
            }
        }

        bool Component::hasLocalSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            String value    = inValue.trim();
            bool   bIsHover = false;
            bool   bIsFocus = false;
            bool   bIsDrag  = false;

            while (true)
            {
                const std::size_t hoverAt = value.find(Style::PSEUDO_CLASS_HOVER);
                const std::size_t focusAt = value.find(Style::PSEUDO_CLASS_FOCUS);
                const std::size_t dragAt  = value.find(Style::PSEUDO_CLASS_DRAG);

                std::size_t at    = String::npos;
                const char* token = nullptr;
                bool*       flag  = nullptr;

                auto consider = [&](std::size_t inAt, const char* inToken, bool& inFlag)
                {
                    if (inAt != String::npos && (at == String::npos || inAt < at))
                    {
                        at    = inAt;
                        token = inToken;
                        flag  = &inFlag;
                    }
                };

                consider(hoverAt, Style::PSEUDO_CLASS_HOVER, bIsHover);
                consider(focusAt, Style::PSEUDO_CLASS_FOCUS, bIsFocus);
                consider(dragAt, Style::PSEUDO_CLASS_DRAG, bIsDrag);

                if (!token)
                {
                    break;
                }

                *flag = true;
                value = value.substr(0, at) + value.substr(at + std::strlen(token));
            }

            if (bIsHover && !isHovered())
            {
                return false;
            }

            if (bIsFocus && !isFocused())
            {
                return false;
            }

            if (bIsDrag && !isDragging())
            {
                return false;
            }

            value = value.trim();

            if (value.isEmpty() || value.equals(Style::INCLUSIVE_SELECTOR))
            {
                return true;
            }

            String              tag;
            String              id;
            std::vector<String> classes;

            std::size_t cursor = value.firstOfChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR);

            if (cursor == String::npos)
            {
                tag = value;
            }
            else
            {
                if (cursor > 0)
                {
                    tag = value.substr(0, cursor);
                }

                value = value.substr(cursor);
            }

            while (value.startsWithChars(Style::CLASS_SELECTOR, Style::ID_SELECTOR))
            {
                const char prefix = value.at(0);

                std::size_t next = String::npos;

                for (std::size_t i = 1; i < value.size(); i++)
                {
                    const char c = value.at(i);

                    if (c == Style::CLASS_SELECTOR || c == Style::ID_SELECTOR)
                    {
                        next = i;
                        break;
                    }
                }

                const std::size_t length = (next == String::npos) ? value.size() : next;

                const String token = value.substr(0, length);

                if (prefix == Style::CLASS_SELECTOR)
                {
                    classes.push_back(token);
                }
                else if (prefix == Style::ID_SELECTOR)
                {
                    id = token;
                }

                if (next == String::npos)
                {
                    break;
                }

                value = value.substr(next);
            }

            if (!tag.isEmpty() && !tag.equals(getTag()))
            {
                return false;
            }

            if (!id.isEmpty())
            {
                if (!id.equals(Style::ID_SELECTOR + getId()))
                {
                    return false;
                }
            }

            const ClassList classList = getClassList();
            for (const String& className : classes)
            {
                if (classList.find(className) == classList.end())
                {
                    return false;
                }
            }

            return true;
        }

        bool Component::hasSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            std::vector<String> parts = inValue.split(Style::SELECTOR_SEPARATOR_SPACE);
            if (parts.empty())
            {
                return false;
            }

            if (!hasLocalSelector(parts.back().trim()))
            {
                return false;
            }

            if (parts.size() == 1)
            {
                return true;
            }

            Component* ancestor = m_parent;
            int        index    = static_cast<int>(parts.size()) - 2;
            while (ancestor && ancestor != this && index >= 0)
            {
                if (ancestor->hasLocalSelector(parts[index].trim()))
                {
                    index--;
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            return index < 0;
        }

        bool Component::matchesCompiledSelector(const StyleCompiledSelector& inSelector) const
        {
            if (inSelector.chain.empty())
            {
                return false;
            }

            if (!matchesCompiledPart(inSelector.chain.back()))
            {
                return false;
            }

            if (inSelector.chain.size() == 1)
            {
                return true;
            }

            Component* ancestor = m_parent;
            int        index    = static_cast<int>(inSelector.chain.size()) - 2;

            while (ancestor && ancestor != this && index >= 0)
            {
                if (ancestor->matchesCompiledPart(inSelector.chain[static_cast<std::size_t>(index)]))
                {
                    index--;
                }

                Component* next = ancestor->m_parent;
                if (!next || next == ancestor)
                {
                    break;
                }

                ancestor = next;
            }

            return index < 0;
        }

        bool Component::matchesCompiledPart(const StyleSelectorPart& inPart) const
        {
            if (inPart.bCanHover && !isHovered())
            {
                return false;
            }

            if (inPart.bCanFocus && !isFocused())
            {
                return false;
            }

            if (inPart.bCanDrag && !isDragging())
            {
                return false;
            }

            if (!inPart.tag.isEmpty() && !inPart.tag.equals(getTag()))
            {
                return false;
            }

            if (!inPart.id.isEmpty() && !inPart.id.equals(Style::ID_SELECTOR + getId()))
            {
                return false;
            }

            if (!inPart.classes.empty())
            {
                const ClassList classList = getClassList();
                for (const String& className : inPart.classes)
                {
                    if (classList.find(className) == classList.end())
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        ReflectionFieldAccessor Component::getField(const String& inId) const
        {
            if (inId.isEmpty())
            {
                return {};
            }

            const auto& local = m_variables.find(inId);

            if (local != m_variables.end())
            {
                return local->second;
            }

            const std::vector<String> parts = inId.split('.');
            if (parts.size() > 1)
            {
                const auto& variable = m_variables.find(parts.at(0));

                if (variable != m_variables.end())
                {
                    const ReflectionFieldAccessor& base = variable->second;

                    if (!base.isValid() || !base.typeIndex.has_value())
                    {
                        return {};
                    }

                    const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(base.typeIndex.value());

                    if (!type)
                    {
                        return {};
                    }

                    const String            subPath = inId.substr(parts.at(0).size() + 1);
                    ReflectionFieldAccessor result  = type->resolve(subPath);

                    if (!result.isValid())
                    {
                        return {};
                    }

                    if (base.boundInstance != nullptr)
                    {
                        result.boundInstance = base.boundInstance;
                        result.bNeedsDeref   = false;
                    }

                    return result;
                }
            }

            if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(*this)))
            {
                const ReflectionFieldAccessor result = type->resolve(inId);

                if (result.isValid())
                {
                    return result;
                }
            }

            return {};
        }

        ReflectionTypeMethod Component::getMethod(const String& inValue) const
        {
            return getMethod(inValue, this);
        }

        bool Component::hasRoot() const
        {
            return m_root != nullptr;
        }

        Component* Component::getRoot() const
        {
            return m_root;
        }

        void Component::setRoot(Component* inComponent)
        {
            if (inComponent == this)
            {
                return;
            }

            m_root = inComponent;

            for (Component* child : m_children)
            {
                child->setRoot(m_root);
            }
        }

        bool Component::hasParent() const
        {
            return m_parent != nullptr && !isRoot();
        }

        Component* Component::getParent() const
        {
            return isRoot() ? nullptr : m_parent;
        }

        void Component::setParent(Component* inComponent)
        {
            if (inComponent == this)
            {
                return;
            }

            m_parent = inComponent;

            onAdoption(inComponent);
        }

        const Component* Component::getContainingBlock() const
        {
            if (isRoot() || !hasParent())
            {
                return this;
            }

            if (!m_style.isPosition(StylePosition::Absolute))
            {
                return m_parent;
            }

            for (const Component* ancestor = m_parent; ancestor != nullptr; ancestor = ancestor->getParent())
            {
                if (ancestor->isRoot() || ancestor->getStyle().isPositioned())
                {
                    return ancestor;
                }
            }

            return hasRoot() ? m_root : this;
        }

        bool Component::hasNeighbours() const
        {
            if (isRoot())
            {
                return false;
            }

            std::vector<const Component*> neighbours;

            for (const Component* children : m_parent->getChildren())
            {
                if (this == children)
                {
                    continue;
                }

                neighbours.push_back(children);
            }

            return neighbours.size() > 0;
        }

        Component* Component::getNeighbour(int inJumps) const
        {
            if (isRoot())
            {
                return nullptr;
            }

            const std::vector<Component*>& neighbours = m_parent->getChildren();

            const std::size_t location = std::find_if(
                                             neighbours.begin(),
                                             neighbours.end(),
                                             [&](Component* children) { return children == this; }
                                         ) -
                                         neighbours.begin();

            return neighbours.at(
                std::clamp(
                    static_cast<std::size_t>(location + inJumps),
                    static_cast<std::size_t>(0),
                    neighbours.size() - 1
                )
            );
        }

        bool Component::hasChildren() const
        {
            return !m_children.empty();
        }

        const std::vector<Component*>& Component::getChildren() const
        {
            return m_children;
        }

        const std::vector<Component*>& Component::getPaintChildren() const
        {
            return m_paintChildren;
        }

        std::vector<Component*> Component::getChildrenFlat() const
        {
            std::vector<Component*> result;
            appendChildrenFlat(result);

            return result;
        }

        void Component::appendChildrenFlat(std::vector<Component*>& outChildren) const
        {
            std::unordered_set<const Component*> visited;
            visited.insert(this);

            std::vector<const Component*> stack;
            stack.push_back(this);

            while (!stack.empty())
            {
                const Component* node = stack.back();
                stack.pop_back();

                const std::vector<Component*> children = node->m_paintChildren;
                std::vector<Component*>       next;
                next.reserve(children.size());

                for (Component* child : children)
                {
                    if (!child || !visited.insert(child).second)
                    {
                        continue;
                    }

                    outChildren.push_back(child);
                    next.push_back(child);
                }

                for (auto it = next.rbegin(); it != next.rend(); it++)
                {
                    stack.push_back(*it);
                }
            }
        }

        void Component::markStyleDirty()
        {
            setFlag(ComponentDirty::Style);
        }

        void Component::markLayoutDirty()
        {
            setFlag(ComponentDirty::Layout);
            markPaintDirty();
        }

        void Component::markStyleDirtySubtree()
        {
            markStyleDirty();

            for (Component* child : m_children)
            {
                child->markStyleDirtySubtree();
            }
        }

        void Component::markLayoutDirtySubtree()
        {
            markLayoutDirty();

            for (Component* child : m_children)
            {
                child->markLayoutDirtySubtree();
            }
        }

        void Component::markPaintDirty()
        {
            setFlag(ComponentDirty::Paint);
        }

        void Component::markPaintDirtySubtree()
        {
            markPaintDirty();

            for (Component* child : m_paintChildren)
            {
                if (child)
                {
                    child->markPaintDirtySubtree();
                }
            }
        }

        void Component::paint(const ComponentPaintContext& inContext)
        {
            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            setFlag(ComponentDirty::Paint, false);

            m_draw.position = getTranslation() - inContext.scroll;
            m_draw.opacity  = inContext.opacity * m_style.opacity.get();
            m_draw.depth    = isRoot() ? 0.0f : inContext.depth + m_style.zIndex.get() + 0.1f;

            const float blur  = m_style.filter.blur.get();
            m_draw.filterBlur = blur > 0.0f ? std::sqrt((inContext.filterBlur * inContext.filterBlur) + (blur * blur))
                                            : inContext.filterBlur;

            const StyleTransform xform = m_style.getTransform();
            m_draw.matrix = xform.isIdentity() ? inContext.world : inContext.world * computeLocalPaintMatrix(xform);

            Bounds2D box;
            box.set(m_draw.position.y, m_draw.position.x, m_draw.position.y + m_size.y, m_draw.position.x + m_size.x);
            box.transform(m_draw.matrix);
            m_draw.bounds = box;

            const bool bEscapes = escapesOverflow();

            m_draw.clip = bEscapes ? Bounds2D::unconstrained() : inContext.clip;

            std::vector<const Component*> roundedAncestors;
            if (!bEscapes && inContext.roundedAncestors)
            {
                roundedAncestors = *inContext.roundedAncestors;
            }

            paintRadius(roundedAncestors);
            paintRoundClips(roundedAncestors);

            setCulled(isCulledByAncestor());

            ComponentPaintContext context;
            context.world            = m_draw.matrix;
            context.clip             = m_draw.clip;
            context.scroll           = inContext.scroll + getScrollOffset();
            context.opacity          = m_draw.opacity;
            context.filterBlur       = m_draw.filterBlur;
            context.depth            = m_draw.depth;
            context.roundedAncestors = &roundedAncestors;

            if (!bEscapes && m_style.isClippingOverflow())
            {
                context.clip = context.clip.intersect(m_draw.bounds);
            }

            const bool bIsRoundedClipper = m_style.isClippingOverflow() && !m_style.radius.isZero();

            if (bIsRoundedClipper)
            {
                roundedAncestors.push_back(this);
            }

            for (Component* child : m_paintChildren)
            {
                if (!child || child == this)
                {
                    continue;
                }

                child->paint(context);
            }
        }

        void Component::paintRadius(const std::vector<const Component*>& inRoundedAncestors)
        {
            m_draw.radiusX = m_style.radius.horizontal();
            m_draw.radiusY = m_style.radius.vertical();

            for (auto it = inRoundedAncestors.rbegin(); it != inRoundedAncestors.rend(); it++)
            {
                const Component* ancestor = *it;
                const Style&     style    = ancestor->getStyle();

                const Bounds2D box     = ancestor->getDrawBounds();
                const Vec4     radiusX = style.radius.horizontal();
                const Vec4     radiusY = style.radius.vertical();
                const float    insetL  = style.padding.left.get() + style.border.paintedLeft();
                const float    insetR  = style.padding.right.get() + style.border.paintedRight();
                const float    insetT  = style.padding.top.get() + style.border.paintedTop();
                const float    insetB  = style.padding.bottom.get() + style.border.paintedBottom();
                const float eps = std::max(2.5f, std::max(std::max(insetL, insetR), std::max(insetT, insetB)) + 1.0f);

                const auto flush = [eps](float inLeft, float inRight) { return std::abs(inLeft - inRight) <= eps; };

                const bool flushLeft =
                    flush(m_draw.bounds.left, box.left) || flush(m_draw.bounds.left, box.left + insetL);
                const bool flushRight =
                    flush(m_draw.bounds.right, box.right) || flush(m_draw.bounds.right, box.right - insetR);
                const bool flushTop = flush(m_draw.bounds.top, box.top) || flush(m_draw.bounds.top, box.top + insetT);
                const bool flushBottom =
                    flush(m_draw.bounds.bottom, box.bottom) || flush(m_draw.bounds.bottom, box.bottom - insetB);

                if (flushTop && flushLeft)
                {
                    m_draw.radiusX.x = std::max(m_draw.radiusX.x, radiusX.x);
                    m_draw.radiusY.x = std::max(m_draw.radiusY.x, radiusY.x);
                }

                if (flushTop && flushRight)
                {
                    m_draw.radiusX.y = std::max(m_draw.radiusX.y, radiusX.y);
                    m_draw.radiusY.y = std::max(m_draw.radiusY.y, radiusY.y);
                }

                if (flushBottom && flushRight)
                {
                    m_draw.radiusX.z = std::max(m_draw.radiusX.z, radiusX.z);
                    m_draw.radiusY.z = std::max(m_draw.radiusY.z, radiusY.z);
                }

                if (flushBottom && flushLeft)
                {
                    m_draw.radiusX.w = std::max(m_draw.radiusX.w, radiusX.w);
                    m_draw.radiusY.w = std::max(m_draw.radiusY.w, radiusY.w);
                }
            }
        }

        void Component::paintRoundClips(const std::vector<const Component*>& inRoundedAncestors)
        {
            m_draw.innerClip        = Vec4::Sentinel();
            m_draw.innerClipRadiusX = Vec4::Zero();
            m_draw.innerClipRadiusY = Vec4::Zero();
            m_draw.outerClip        = Vec4::Sentinel();
            m_draw.outerClipRadiusX = Vec4::Zero();
            m_draw.outerClipRadiusY = Vec4::Zero();

            std::uint32_t filled = 0;

            for (auto it = inRoundedAncestors.rbegin(); it != inRoundedAncestors.rend() && filled < 2; it++)
            {
                const Component* ancestor = *it;
                const Bounds2D   box      = ancestor->getDrawBounds();
                const Vec4       radiusX  = ancestor->getStyle().radius.horizontal();
                const Vec4       radiusY  = ancestor->getStyle().radius.vertical();
                const Vec4       clip     = Vec4(box.left, box.top, box.right, box.bottom);

                if (filled == 0)
                {
                    m_draw.innerClip        = clip;
                    m_draw.innerClipRadiusX = radiusX;
                    m_draw.innerClipRadiusY = radiusY;
                }
                else
                {
                    m_draw.outerClip        = clip;
                    m_draw.outerClipRadiusX = radiusX;
                    m_draw.outerClipRadiusY = radiusY;
                }

                filled++;
            }
        }

        void Component::setPeripherals(const std::vector<Component*>& inPeripherals)
        {
            if (m_peripherals == inPeripherals)
            {
                return;
            }

            m_peripherals = inPeripherals;

            rebuildPaintChildren();
        }

        Vec2 Component::getScrollOffset() const
        {
            return Vec2::Zero();
        }

        Vec2 Component::getScrollBarGutter() const
        {
            return Vec2::Zero();
        }

        bool Component::hideIfDirective()
        {
            const String& attribute = getAttribute(IF_DIRECTIVE_KEYWORD);
            if (attribute.isEmpty())
            {
                return false;
            }

            const bool bShouldShow = parseText(attribute).equals("true", "1");
            if (bShouldShow)
            {
                if (m_style.isDisplay(StyleDisplay::None))
                {
                    markStyleDirty();
                    markLayoutDirty();
                }

                return false;
            }

            m_style.display.set(StyleDisplay::None);
            setCulled(false);
            setFlag(ComponentDirty::Style, false);
            setFlag(ComponentDirty::Layout, false);

            return true;
        }

        Component* Component::getHitAt(const Vec2& inLocation) const
        {
            Component* hit = nullptr;

            std::vector<const Component*>        stack;
            std::unordered_set<const Component*> visited;
            stack.push_back(this);
            visited.insert(this);

            auto consider = [&](Component* candidate)
            {
                if (!candidate || !candidate->isDisplayable() || candidate->isCulled())
                {
                    return;
                }

                if (!candidate->getDrawBounds().contains(inLocation))
                {
                    return;
                }

                if (!candidate->containsPoint(inLocation))
                {
                    return;
                }

                if (!hit || candidate->getDepth() >= hit->getDepth())
                {
                    hit = candidate;
                }
            };

            while (!stack.empty())
            {
                const Component* node = stack.back();
                stack.pop_back();

                if (node != this && node->isCulled())
                {
                    continue;
                }

                const std::vector<Component*> children = node->m_paintChildren;
                for (auto it = children.rbegin(); it != children.rend(); it++)
                {
                    Component* child = *it;
                    if (!child || !child->isDisplayable() || !visited.insert(child).second)
                    {
                        continue;
                    }

                    stack.push_back(child);
                }

                if (node != this)
                {
                    consider(const_cast<Component*>(node));
                }
            }

            return hit;
        }

        bool Component::containsPoint(const Vec2& inLocation) const
        {
            if (!isDisplayable())
            {
                return false;
            }

            if (!getDrawBounds().contains(inLocation))
            {
                return false;
            }

            Vec2             local  = inLocation;
            const Mat3&      paint  = getPaintMatrix();
            const glm::mat3& matrix = paint;

            const bool bIdentity = std::fabs(matrix[0][0] - 1.0f) <= 0.0001f &&
                                   std::fabs(matrix[1][1] - 1.0f) <= 0.0001f &&
                                   std::fabs(matrix[2][2] - 1.0f) <= 0.0001f && std::fabs(matrix[0][1]) <= 0.0001f &&
                                   std::fabs(matrix[0][2]) <= 0.0001f && std::fabs(matrix[1][0]) <= 0.0001f &&
                                   std::fabs(matrix[1][2]) <= 0.0001f && std::fabs(matrix[2][0]) <= 0.0001f &&
                                   std::fabs(matrix[2][1]) <= 0.0001f;

            if (!bIdentity)
            {
                const Mat3 inverse = glm::inverse(matrix);
                const Vec3 mapped  = inverse * Vec3(inLocation.x, inLocation.y, 1.0f);

                local.x = mapped.x;
                local.y = mapped.y;
            }

            const Vec2 position = getDrawPosition();
            Bounds2D   box;
            box.left   = position.x;
            box.top    = position.y;
            box.right  = position.x + m_size.x;
            box.bottom = position.y + m_size.y;

            if (!box.containsRounded(local, m_style.radius.horizontal(), m_style.radius.vertical()))
            {
                return false;
            }

            if (!getOverflowClip().contains(inLocation))
            {
                return false;
            }

            if (escapesOverflow())
            {
                return true;
            }

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                if (ancestor->getStyle().isClippingOverflow() && !ancestor->getDrawBounds().containsRounded(
                                                                     inLocation,
                                                                     ancestor->getStyle().radius.horizontal(),
                                                                     ancestor->getStyle().radius.vertical()
                                                                 ))
                {
                    return false;
                }

                if (ancestor->escapesOverflow() || ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return true;
        }

        bool Component::broadcastEvent(const WindowEvent& inEvent)
        {
            std::vector<Component*> stack;
            stack.reserve(m_children.size());

            for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
            {
                if (*it)
                {
                    stack.push_back(*it);
                }
            }

            while (!stack.empty())
            {
                Component* child = stack.back();
                stack.pop_back();

                if (child->isCulled())
                {
                    continue;
                }

                if (child->onEvent(inEvent))
                {
                    return true;
                }

                for (auto it = child->m_children.rbegin(); it != child->m_children.rend(); it++)
                {
                    if (*it)
                    {
                        stack.push_back(*it);
                    }
                }
            }

            return false;
        }

        bool Component::bubbleEvent(const WindowEvent& inEvent, const Vec2& inLocation)
        {
            Component* node = getHitAt(inLocation);
            while (node && node != this)
            {
                if (node->onEvent(inEvent))
                {
                    return true;
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return false;
        }

        void Component::addChildren(const XmlNode& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            bool bWasAdopted = false;
            for (const auto& child : inNode.getChildren())
            {
                if (isContentSlot(child))
                {
                    addProjectedContent(child);

                    continue;
                }

                Component* component = create(child);
                if (!component)
                {
                    continue;
                }

                if (adoptChild(component))
                {
                    bWasAdopted = true;
                }
            }

            if (bWasAdopted)
            {
                rebuildPaintChildren();
                markLayoutDirty();
            }
        }

        bool Component::adoptChild(Component* inComponent, std::size_t inIndex)
        {
            if (!canAdopt(inComponent))
            {
                return false;
            }

            inComponent->setRoot(m_root);
            inComponent->setParent(this);
            inComponent->setStyleFile(inComponent->m_bHasOwnStyle ? inComponent->m_styleFile : m_styleFile);

            if (inIndex >= m_children.size())
            {
                m_children.push_back(inComponent);
            }
            else
            {
                m_children.insert(m_children.begin() + static_cast<std::ptrdiff_t>(inIndex), inComponent);
            }

            onAdopted(inComponent);

            return true;
        }

        void Component::addChild(Component* inComponent, std::size_t inIndex)
        {
            if (!adoptChild(inComponent, inIndex))
            {
                return;
            }

            rebuildPaintChildren();
            markLayoutDirty();
        }

        void Component::releaseChild(Component* inComponent)
        {
            if (!inComponent)
            {
                return;
            }

            const auto found = std::find(m_children.begin(), m_children.end(), inComponent);
            if (found == m_children.end())
            {
                return;
            }

            m_children.erase(found);

            if (inComponent->m_parent == this)
            {
                inComponent->m_parent = nullptr;
            }

            rebuildPaintChildren();
            markLayoutDirty();
        }

        void Component::removeChild(Component* inComponent)
        {
            if (!inComponent)
            {
                return;
            }

            const auto found = std::find(m_children.begin(), m_children.end(), inComponent);
            if (found == m_children.end())
            {
                return;
            }

            m_children.erase(found);

            if (inComponent->m_parent == this)
            {
                inComponent->m_parent = nullptr;
            }

            delete inComponent;
            rebuildPaintChildren();
            markLayoutDirty();
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
            const bool  bIsRow   = m_style.flex.direction.get() == StyleFlexDirection::Row;
            const bool  bCanWrap = m_style.flex.wrap.get() == StyleFlexWrap::Wrap;
            const float mainGap  = bIsRow ? m_style.gap.left.get() : m_style.gap.top.get();
            const float crossGap = bIsRow ? m_style.gap.top.get() : m_style.gap.left.get();
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
                    if (bIsRow)
                    {
                        const float percent =
                            percentOfBasis(style.width.value.getRaw(), wrapMain, SizeDirection::Horizontal);
                        if (percent >= 0.0f)
                        {
                            size.x = percent;
                        }
                    }
                    else
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
            Vec2 result = Vec2::Zero();

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
            if (m_style.display.get() == StyleDisplay::Flex)
            {
                return getChildrenContentSizeFlex();
            }

            return getChildrenContentSizeBlock();
        }

        Vec2 Component::getChildIntrinsicSize(const Component* inChild) const
        {
            const Style& style = inChild->getStyle();
            Vec2         size  = inChild->getContentSize();

            const bool bIsWidthAuto       = isWidthAuto(style);
            const bool bIsHeightAuto      = isHeightAuto(style);
            const bool bIsWidthIntrinsic  = bIsWidthAuto || isWidthIntrinsicAuto(style, m_style);
            const bool bIsHeightIntrinsic = bIsHeightAuto || isHeightIntrinsicAuto(style, m_style);

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
                std::max(0.0f, m_size.x - m_style.insetHorizontal()),
                std::max(0.0f, m_size.y - m_style.insetVertical())
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

            if (!inChild || !isFlexNowrap(m_style))
            {
                return inner;
            }

            const bool  bIsRow    = m_style.flex.direction.get() == StyleFlexDirection::Row;
            const float mainGap   = bIsRow ? m_style.gap.left.get() : m_style.gap.top.get();
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
            if (!inChild || !isFlexNowrap(m_style))
            {
                return 0.0f;
            }

            const bool  bIsRow  = m_style.flex.direction.get() == StyleFlexDirection::Row;
            const float mainGap = bIsRow ? m_style.gap.left.get() : m_style.gap.top.get();

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
            if (!inChild || !isFlexNowrap(m_style))
            {
                return 0;
            }

            const bool bIsRow = m_style.flex.direction.get() == StyleFlexDirection::Row;
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
                    if (sibling == inChild)
                    {
                        bAfter = true;
                    }
                    else
                    {
                        continue;
                    }
                }

                const Style& style = sibling->getStyle();
                if (bIsRow)
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
                else
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
            m_style.clampSize(width, height);
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
            m_style.clampSize(width, height);

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

        Vec2 Component::getDrawPosition() const
        {
            return m_draw.position;
        }

        Vec2 Component::getTransformPivot() const
        {
            return getDrawPosition() + getStyle().getTransformOrigin(getBorderSize());
        }

        const Mat3& Component::getPaintMatrix() const
        {
            return m_draw.matrix;
        }

        Mat3 Component::computeLocalPaintMatrix(const StyleTransform& inTransform) const
        {
            const Vec2  origin  = getTransformPivot();
            const float radians = glm::radians(inTransform.rotation);
            const float cosine  = glm::cos(radians);
            const float sine    = glm::sin(radians);

            const Mat3 toOrigin(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -origin.x, -origin.y, 1.0f);
            const Mat3 fromOrigin(
                1.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f,
                0.0f,
                origin.x + inTransform.translation.x,
                origin.y + inTransform.translation.y,
                1.0f
            );
            const Mat3 linear(
                cosine * inTransform.scale.x,
                sine * inTransform.scale.x,
                0.0f,
                -sine * inTransform.scale.y,
                cosine * inTransform.scale.y,
                0.0f,
                0.0f,
                0.0f,
                1.0f
            );

            return fromOrigin * linear * toOrigin;
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

        const Bounds2D& Component::getBounds() const
        {
            return Transformable2D::getBounds();
        }

        const Bounds2D& Component::getDrawBounds() const
        {
            return m_draw.bounds;
        }

        const Bounds2D& Component::getOverflowClip() const
        {
            return m_draw.clip;
        }

        void Component::getOverflowRoundClips(
            Vec4& outFirst,
            Vec4& outFirstRadiusX,
            Vec4& outFirstRadiusY,
            Vec4& outSecond,
            Vec4& outSecondRadiusX,
            Vec4& outSecondRadiusY
        ) const
        {
            outFirst         = m_draw.innerClip;
            outFirstRadiusX  = m_draw.innerClipRadiusX;
            outFirstRadiusY  = m_draw.innerClipRadiusY;
            outSecond        = m_draw.outerClip;
            outSecondRadiusX = m_draw.outerClipRadiusX;
            outSecondRadiusY = m_draw.outerClipRadiusY;
        }

        void Component::getPaintRadius(Vec4& outRadiusX, Vec4& outRadiusY) const
        {
            outRadiusX = m_draw.radiusX;
            outRadiusY = m_draw.radiusY;
        }

        bool Component::hasPrimitive() const
        {
            return !m_primitive.isEmpty();
        }

        const Primitive& Component::getPrimitive() const
        {
            return m_primitive;
        }

        void Component::clearPrimitive()
        {
            m_primitive.clear();
        }

        void Component::setPrimitive(const Primitive& inPrimitive)
        {
            m_primitive = inPrimitive;
            m_primitive.seal();
        }

        String Component::parseText(const String& inValue) const
        {
            if (inValue.isEmpty() || !isReference(inValue))
            {
                return inValue;
            }

            const std::size_t openPosition = inValue.firstOf(REFERENCE_VALUE_OPENING);
            if (openPosition == String::npos)
            {
                return inValue;
            }

            const std::size_t closePosition = inValue.firstOf(REFERENCE_VALUE_CLOSING, openPosition + 2);
            if (closePosition == String::npos)
            {
                return inValue;
            }

            const std::size_t valueStart = openPosition + 2;

            String result;

            const String prefix = inValue.substr(0, openPosition);
            if (!prefix.isEmpty())
            {
                result.append(parseText(prefix));
            }

            const String value = inValue.substr(valueStart, closePosition - valueStart).trim();
            if (!value.isEmpty())
            {
                result.append(parseReference(value));
            }

            const String suffix = inValue.substr(closePosition + 2);
            if (!suffix.isEmpty())
            {
                result.append(parseText(suffix));
            }

            return result;
        }

        void Component::load(const FileSystem::Path& inTemplate, const FileSystem::Path& inStyle)
        {
            if (!inStyle.isEmpty())
            {
                importStyleFile(inStyle);
            }

            if (inTemplate.isEmpty())
            {
                return;
            }

            thread_local std::vector<String> loading;

            const String source = inTemplate.toString();
            for (const String& path : loading)
            {
                if (path.equals(source))
                {
                    throw std::runtime_error("Cyclic Grid component include [" + source + "]");
                }
            }

            Loading guard(loading, source);

            XmlDocument document = Xml::load(inTemplate);
            if (document.empty())
            {
                throw std::runtime_error("UI document " + source + " does not have any components");
            }

            const XmlNode root = document.getFirstChild();
            if (!(root.getParent() == root.getRoot() && root.getNextSibling().isEmpty()))
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            std::vector<Component*> projected = m_children;
            m_children.clear();

            Scope      scope(this);
            Projection projection(projected);

            if (String(root.getName()).equals(getTag()))
            {
                parse(root);
                setId(getAttribute(ID_ATTRIBUTE_NAME));
                setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));
                addChildren(getSource());
            }
            else if (Component* wrapper = create(root))
            {
                addChild(wrapper);
            }

            for (Component* child : projected)
            {
                addChild(child);
            }
        }

        void Component::addProjectedContent(const XmlNode& inSlot)
        {
            if (!g_projected || g_projected->empty())
            {
                return;
            }

            const String select = inSlot.parseString(CONTENT_SELECT_ATTRIBUTE_NAME, String::empty()).trim();

            std::vector<Component*> leftover;
            leftover.reserve(g_projected->size());

            for (Component* child : *g_projected)
            {
                if (!child)
                {
                    continue;
                }

                if (select.isEmpty() || child->hasLocalSelector(select))
                {
                    addChild(child);

                    continue;
                }

                leftover.push_back(child);
            }

            *g_projected = std::move(leftover);
        }

        void Component::refreshClassName()
        {
            const auto found = m_attributes.find(CLASS_ATTRIBUTE_NAME);
            if (found == m_attributes.end())
            {
                return;
            }

            const String className = parseText(found->second);
            if (className.equals(m_className))
            {
                return;
            }

            setClassName(className);
        }

        void Component::refreshId()
        {
            const auto found = m_attributes.find(ID_ATTRIBUTE_NAME);
            if (found == m_attributes.end())
            {
                return;
            }

            const String id = parseText(found->second).trim();
            if (id.equals(m_id))
            {
                return;
            }

            setId(id);
        }

        void Component::refreshStyle()
        {
            m_style.refresh();
        }

        void Component::refreshBounds()
        {
            m_bounds.set(0.0f, 0.0f, m_size.y, m_size.x);
            m_bounds.transform(getMatrix());
        }

        void Component::resetFlowCursor()
        {
            setCursor(getPosition().x + m_style.insetLeft(), getPosition().y + m_style.insetTop());
            m_scratch = 0.0f;
        }

        void Component::reflowChildPositions()
        {
            resetFlowCursor();

            for (Component* child : m_children)
            {
                if (!child || child->m_style.isDisplay(StyleDisplay::None))
                {
                    continue;
                }

                child->refreshPosition();
                child->refreshBounds();
                child->reflowChildPositions();
            }
        }

        const StyleKeyframe::List* Component::findKeyframes(const String& inName) const
        {
            std::vector<StyleFile*> files;

            auto addFile = [&](StyleFile* inFile)
            {
                if (!inFile)
                {
                    return;
                }

                if (std::find(files.begin(), files.end(), inFile) != files.end())
                {
                    return;
                }

                files.push_back(inFile);
            };

            std::vector<StyleFile*> ancestors;
            for (const Component* ancestor = m_parent; ancestor && ancestor != this; ancestor = ancestor->m_parent)
            {
                addFile(ancestor->m_styleFile);

                if (ancestor->isRoot())
                {
                    break;
                }
            }

            for (auto iterator = ancestors.rbegin(); iterator != ancestors.rend(); iterator++)
            {
                addFile(*iterator);
            }

            addFile(m_styleFile);

            if (m_bHasOwnStyle)
            {
                addFile(m_styles.get());
            }

            for (auto iterator = files.rbegin(); iterator != files.rend(); iterator++)
            {
                if ((*iterator)->hasKeyframes(inName))
                {
                    return &(*iterator)->getKeyframes(inName);
                }
            }

            return nullptr;
        }

        Drift::Clip Component::makeAnimationClip(const StyleKeyframe::List& inKeyframes) const
        {
            Drift::Clip clip(m_style.animation.name);
            clip.duration   = Time::fromMilliseconds(m_style.animation.duration);
            clip.iterations = m_style.animation.iterations;
            clip.loop       = m_style.animation.bIsAlternate
                                  ? Drift::Loop::PingPong
                                  : (m_style.animation.iterations == 1 ? Drift::Loop::Once : Drift::Loop::Repeat);

            std::unordered_map<String, Drift::Track> tracks;

            for (const StyleKeyframe& frame : inKeyframes)
            {
                Style snapshot;
                snapshot.setParent(this);
                snapshot.setProperties(frame.properties);
                snapshot.refresh();

                const Time time = clip.duration * frame.offset;

                for (const auto& [name, value] : frame.properties)
                {
                    StylePropertyId id = StylePropertyId::Count;

                    if (!StylePropertyTable::find(name, id))
                    {
                        continue;
                    }

                    float values[MAX_PROPERTY_ARITY] = {};

                    if (!snapshot.readAnimated(id, values))
                    {
                        continue;
                    }

                    const std::vector<float> parsed(values, values + StylePropertyTable::get(id).arity);

                    auto found = tracks.find(name);

                    if (found == tracks.end())
                    {
                        tracks.insert({name, Drift::Track(name)});
                        found = tracks.find(name);
                    }

                    found->second.addKeyframe(time, parsed, m_style.animation.easing);
                }
            }

            for (const auto& [name, track] : tracks)
            {
                clip.addTrack(track);
            }

            return clip;
        }

        bool Component::isReference(const String& inValue) const
        {
            const bool bHasOpening = inValue.firstOf(REFERENCE_VALUE_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(REFERENCE_VALUE_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        String Component::parseReference(const String& inValue) const
        {
            if (inValue.startsWith('!') && !inValue.startsWith("!="))
            {
                const String inner = parseReference(inValue.substr(1).trim());

                return inner.equals("true", "1") ? "false" : "true";
            }

            const std::size_t equals = inValue.firstOf("==");
            if (equals != String::npos)
            {
                const String left  = parseReference(inValue.substr(0, equals).trim());
                const String right = parseReference(inValue.substr(equals + 2).trim());

                return left.equals(right) ? "true" : "false";
            }

            if (isMethod(inValue))
            {
                return parseMethod(inValue);
            }

            ReflectionFieldAccessor accessor = getField(inValue);

            if (accessor.isValid())
            {
                const void* instance =
                    accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(this);

                return accessor.toString(instance);
            }

            return hasParent() ? m_parent->parseReference(inValue) : inValue;
        }

        String Component::parseMethod(const String& inValue) const
        {
            const std::size_t open = inValue.firstOf(METHOD_PARAMS_OPENING);
            if (open == String::npos)
            {
                return String::empty();
            }

            const String qualified = inValue.substr(0, open).trim();
            if (qualified.isEmpty())
            {
                return String::empty();
            }

            const std::size_t dot      = qualified.lastOf('.');
            const String      receiver = dot == String::npos ? String::empty() : qualified.substr(0, dot);
            const String      name     = dot == String::npos ? qualified : qualified.substr(dot + 1);
            if (name.isEmpty())
            {
                return String::empty();
            }

            for (const Component* node = this; node != nullptr; node = node->hasParent() ? node->getParent() : nullptr)
            {
                const ReflectionTypeInfo* type     = nullptr;
                void*                     instance = nullptr;

                if (receiver.isEmpty())
                {
                    type     = ReflectionTypeRegistry::getInstance().find(typeid(*node));
                    instance = const_cast<Component*>(node);
                }
                else
                {
                    const ReflectionFieldAccessor accessor = node->getField(receiver);
                    if (!accessor.isValid() || !accessor.typeIndex.has_value())
                    {
                        if (node->isRoot())
                        {
                            break;
                        }

                        continue;
                    }

                    type     = ReflectionTypeRegistry::getInstance().find(accessor.typeIndex.value());
                    instance = const_cast<char*>(accessor.address(node));

                    if (name.equals("isEmpty") && accessor.isType<String>())
                    {
                        const String* value = accessor.getValue<String>(node);

                        return (!value || value->isEmpty()) ? "true" : "false";
                    }
                }

                if (type && instance)
                {
                    if (const ReflectionTypeMethodInfo* method = type->findMethod(name))
                    {
                        try
                        {
                            ReflectionTypeMethod call(method);
                            call.bind(instance);
                            populateMethodParams(call, inValue);

                            return method->toString(call.invoke());
                        }
                        catch (const std::exception&)
                        {
                            return String::empty();
                        }
                    }
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            return String::empty();
        }

        void Component::addVariable(const String& inId, const ReflectionFieldAccessor& inValue)
        {
            if (!inValue.isValid())
            {
                m_variables.erase(inId);

                return;
            }

            m_variables[inId] = std::move(inValue);
        }

        Component* Component::cloneTemplate() const
        {
            if (m_source.empty())
            {
                return nullptr;
            }

            Scope      scope(m_importOwner);
            Component* clone = create(m_source);
            if (!clone)
            {
                return nullptr;
            }

            clone->removeAttribute(FOR_DIRECTIVE_KEYWORD);

            if (clone->m_className.isEmpty() && !m_className.isEmpty())
            {
                clone->setClassName(m_className);
            }

            return clone;
        }

        const ReflectionTypeInfo* Component::findImported(const String& inSelector) const
        {
            if (inSelector.isEmpty())
            {
                return nullptr;
            }

            const auto found = m_imports.find(inSelector);

            return found != m_imports.end() ? found->second : nullptr;
        }

        void Component::syncForLoop(
            const String& inVariableId, const ReflectionFieldAccessor& inAccessor, const void* inInstance
        )
        {
            if (!hasParent())
            {
                return;
            }

            Component*        parent = getParent();
            const std::size_t count  = inAccessor.getSize(inInstance);

            if (m_forInstances.empty())
            {
                m_forInstances.push_back(this);
            }

            bool bWasMutated = false;

            while (m_forInstances.size() < count)
            {
                Component* instance = cloneTemplate();
                if (!instance)
                {
                    break;
                }

                const std::vector<Component*>& siblings = parent->getChildren();
                const auto        found = std::find(siblings.begin(), siblings.end(), m_forInstances.back());
                const std::size_t index = found == siblings.end()
                                              ? siblings.size()
                                              : static_cast<std::size_t>(std::distance(siblings.begin(), found) + 1);

                parent->addChild(instance, index);
                m_forInstances.push_back(instance);
                bWasMutated = true;
            }

            while (m_forInstances.size() > std::max(count, static_cast<std::size_t>(1)))
            {
                Component* extra = m_forInstances.back();
                m_forInstances.pop_back();

                if (extra == this)
                {
                    m_forInstances.insert(m_forInstances.begin(), extra);

                    break;
                }

                parent->removeChild(extra);

                bWasMutated = true;
            }

            for (std::size_t i = 0; i < m_forInstances.size(); i++)
            {
                Component* instance = m_forInstances.at(i);

                if (i >= count)
                {
                    instance->m_style.display.set(StyleDisplay::None);
                    instance->m_variables.erase(inVariableId);

                    continue;
                }

                ReflectionFieldAccessor element = inAccessor.getElement(inInstance, i);

                if (!element.isValid())
                {
                    instance->m_style.display.set(StyleDisplay::None);

                    continue;
                }

                instance->addVariable(inVariableId, element);

                const bool bWasHidden = instance->getStyle().isDisplay(StyleDisplay::None);
                instance->m_style.display.set(StyleDisplay::Flex);

                if (bWasHidden)
                {
                    instance->markStyleDirtySubtree();
                    instance->markLayoutDirtySubtree();
                    bWasMutated = true;
                }
            }

            if (bWasMutated)
            {
                parent->markLayoutDirty();
                parent->rebuildPaintChildren();
            }
        }

        bool Component::isMethod(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            const bool bHasOpening = inValue.firstOf(METHOD_PARAMS_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(METHOD_PARAMS_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        ReflectionTypeMethod Component::getMethod(const String& inValue, const Component* inParamContext) const
        {
            String signature = inValue.trim();

            if (isReference(signature))
            {
                signature = signature.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            }

            if (!isMethod(signature))
            {
                return {};
            }

            if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::getInstance().find(typeid(*this)))
            {
                const String name = signature.substr(0, signature.firstOf(METHOD_PARAMS_OPENING));

                if (const ReflectionTypeMethodInfo* method = type->findMethod(name))
                {
                    ReflectionTypeMethod result(method);
                    result.bind(const_cast<Component*>(this));

                    if (inParamContext)
                    {
                        inParamContext->populateMethodParams(result, signature);
                    }

                    return result;
                }
            }

            if (!hasParent())
            {
                return {};
            }

            return m_parent->getMethod(inValue, inParamContext);
        }

        void Component::populateMethodParams(ReflectionTypeMethod& outMethod, const String& inSignature) const
        {
            const std::size_t open  = inSignature.firstOf(METHOD_PARAMS_OPENING);
            const std::size_t close = inSignature.lastOf(METHOD_PARAMS_CLOSING);

            if (open == String::npos || close == String::npos || close <= open)
            {
                return;
            }

            const String paramsRaw = inSignature.substr(open + 1, close - open - 1).trim();
            if (paramsRaw.isEmpty())
            {
                return;
            }

            const ReflectionTypeMethodInfo* info       = outMethod.getInfo();
            std::size_t                     paramIndex = 0;

            for (const String& rawParam : splitMethodParams(paramsRaw))
            {
                const String param = rawParam.trim();
                if (param.isEmpty())
                {
                    continue;
                }

                const String expected =
                    info && paramIndex < info->paramTypes.size() ? info->paramTypes.at(paramIndex) : String::empty();
                paramIndex++;

                if ((param.startsWith("\"") && param.endsWith("\"")) || (param.startsWith("'") && param.endsWith("'")))
                {
                    outMethod.addParam(param.substr(1, param.size() - 2));

                    continue;
                }

                ReflectionFieldAccessor accessor = getField(param);
                const Component*        owner    = this;
                if (!accessor.isValid())
                {
                    for (const Component* node = hasParent() ? getParent() : nullptr; node != nullptr;
                         node                  = node->hasParent() ? node->getParent() : nullptr)
                    {
                        accessor = node->getField(param);
                        if (accessor.isValid())
                        {
                            owner = node;

                            break;
                        }

                        if (node->isRoot())
                        {
                            break;
                        }
                    }
                }

                if (accessor.isValid())
                {
                    const void* instance =
                        accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(owner);

                    if (expected.endsWith('*'))
                    {
                        const void* fieldAddr = accessor.address(instance);
                        void*       pointer   = nullptr;

                        if (fieldAddr)
                        {
                            if (accessor.size == sizeof(void*))
                            {
                                pointer = *reinterpret_cast<void* const*>(fieldAddr);
                            }
                            else
                            {
                                pointer = const_cast<void*>(fieldAddr);
                            }
                        }

                        outMethod.addParam(pointer);

                        continue;
                    }

                    outMethod.addParam(accessor.toString(instance));

                    continue;
                }

                outMethod.addParam(param);
            }
        }

        std::vector<String> Component::splitMethodParams(const String& inValue) const
        {
            std::vector<String> result;

            std::size_t   start            = 0;
            std::uint32_t parenthesisCount = 0;

            for (std::size_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);

                if (character == METHOD_PARAMS_OPENING)
                {
                    parenthesisCount++;

                    continue;
                }

                if (character == METHOD_PARAMS_CLOSING)
                {
                    if (parenthesisCount > 0)
                    {
                        parenthesisCount--;
                    }

                    continue;
                }

                if (character != METHOD_PARAMS_SEPARATOR || parenthesisCount > 0)
                {
                    continue;
                }

                result.push_back(inValue.substr(start, i - start));
                start = i + 1;
            }

            if (start <= inValue.size())
            {
                result.push_back(inValue.substr(start));
            }

            return result;
        }

        bool Component::isCulledByAncestor() const
        {
            if (m_size.x <= 0.0f || m_size.y <= 0.0f)
            {
                return false;
            }

            const Bounds2D clip = getOverflowClip();
            if (clip.isEmpty())
            {
                return true;
            }

            Bounds2D    draw = getDrawBounds();
            const float pad  = std::max(m_size.x, m_size.y) + 64.0f;
            draw.left -= pad;
            draw.top -= pad;
            draw.right += pad;
            draw.bottom += pad;

            return !draw.overlaps(clip);
        }

        void Component::rebuildPaintChildren()
        {
            m_paintChildren.clear();
            m_paintChildren.reserve(m_children.size() + m_peripherals.size());

            auto append = [this](Component* node)
            {
                if (!node || node == this)
                {
                    return;
                }

                for (const Component* ancestor = this; ancestor != nullptr;
                     ancestor                  = ancestor->isRoot() ? nullptr : ancestor->m_parent)
                {
                    if (ancestor == node)
                    {
                        return;
                    }
                }

                for (Component* existing : m_paintChildren)
                {
                    if (existing == node)
                    {
                        return;
                    }
                }

                m_paintChildren.push_back(node);
            };

            for (Component* child : m_children)
            {
                append(child);
            }

            for (Component* peripheral : m_peripherals)
            {
                append(peripheral);
            }
        }
    }
}
