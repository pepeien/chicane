#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Size.hpp"

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
                   inLeft.flexWrap == inRight.flexWrap && inLeft.widthRaw.equals(inRight.widthRaw) &&
                   inLeft.heightRaw.equals(inRight.heightRaw) && inLeft.minWidthRaw.equals(inRight.minWidthRaw) &&
                   inLeft.minHeightRaw.equals(inRight.minHeightRaw) && inLeft.maxWidthRaw.equals(inRight.maxWidthRaw) &&
                   inLeft.maxHeightRaw.equals(inRight.maxHeightRaw) && inLeft.marginL.equals(inRight.marginL) &&
                   inLeft.marginR.equals(inRight.marginR) && inLeft.marginT.equals(inRight.marginT) &&
                   inLeft.marginB.equals(inRight.marginB) && inLeft.paddingL.equals(inRight.paddingL) &&
                   inLeft.paddingR.equals(inRight.paddingR) && inLeft.paddingT.equals(inRight.paddingT) &&
                   inLeft.paddingB.equals(inRight.paddingB);
        }

        LayoutMetrics captureLayoutMetrics(const Style& inStyle)
        {
            return {
                inStyle.display.get(),
                inStyle.position.get(),
                inStyle.align.get(),
                inStyle.flex.direction.get(),
                inStyle.flex.wrap.get(),
                inStyle.width.value.getRaw(),
                inStyle.height.value.getRaw(),
                inStyle.width.min.getRaw(),
                inStyle.height.min.getRaw(),
                inStyle.width.max.getRaw(),
                inStyle.height.max.getRaw(),
                inStyle.margin.left.getRaw(),
                inStyle.margin.right.getRaw(),
                inStyle.margin.top.getRaw(),
                inStyle.margin.bottom.getRaw(),
                inStyle.padding.left.getRaw(),
                inStyle.padding.right.getRaw(),
                inStyle.padding.top.getRaw(),
                inStyle.padding.bottom.getRaw()
            };
        }

        bool isHeightAuto(const Style& inStyle)
        {
            return inStyle.height.isAuto();
        }

        bool isWidthAuto(const Style& inStyle)
        {
            return inStyle.width.isAuto();
        }

        bool isFlexNowrap(const Style& inStyle)
        {
            return inStyle.isDisplay(StyleDisplay::Flex) && inStyle.flex.wrap.get() == StyleFlexWrap::NoWrap;
        }

        Vec2 innerLayoutSize(const Component* inBox)
        {
            if (!inBox)
            {
                return Vec2::Zero();
            }

            const Vec2 content = inBox->getContentSize();

            return {
                std::max(0.0f, content.x),
                std::max(0.0f, content.y)
            };
        }

        String expandStyleBinding(const String& inValue)
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

        Component* Component::create(const pugi::xml_node& inNode)
        {
            if (inNode.empty() || inNode.type() != pugi::node_element || isContentSlot(inNode))
            {
                return nullptr;
            }

            const String              tag = inNode.name();
            const ReflectionTypeInfo* type =
                ReflectionTypeRegistry::getInstance().find(String("Chicane::Grid::") + tag);

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

        bool Component::isContentSlot(const pugi::xml_node& inNode)
        {
            if (inNode.empty() || inNode.type() != pugi::node_element)
            {
                return false;
            }

            return String(inNode.name()).equals(CONTENT_TAG_ID);
        }

        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            m_sourceNode = m_sourceDocument.append_copy(inNode);
            m_attributes = Xml::getAttributes(m_sourceNode);
            cacheAttributeFlags();
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            addChildren(inNode);

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
                    if (m_bSkipForDirective || inValue.isEmpty())
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
              m_directives({}),
              m_variables({}),
              m_style({}),
              m_styleVariables({}),
              m_styleFile(nullptr),
              m_styles(nullptr),
              m_bOwnsStyle(false),
              m_imports({}),
              m_importOwner(nullptr),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_flatChildren({}),
              m_size(Vec2::Zero()),
              m_scale(Vec2::Zero()),
              m_offset(Vec2::Zero()),
              m_cursor(Vec2::Zero()),
              m_scratch(0.0f),
              m_layoutParentWidth(-1.0f),
              m_layoutParentHeight(-1.0f),
              m_layoutParentFontSize(-1.0f),
              m_primitive({}),
              m_attributes({}),
              m_sourceNode(),
              m_forInstances({}),
              m_forVariable(String::empty()),
              m_forSource({}),
              m_bSkipForDirective(false),
              m_bIsHovered(false),
              m_bIsFocused(false),
              m_bIsDragging(false),
              m_bIsStyleDirty(true),
              m_bIsLayoutDirty(true),
              m_bIsCulled(false),
              m_bLaidOutThisFrame(false),
              m_bInsetsApplied(false),
              m_bClassHasBinding(false),
              m_bStyleHasBinding(false),
              m_styleBindingSource(String::empty()),
              m_styleBindingResolved(String::empty()),
              m_bHasIfDirective(false),
              m_bDrawCacheValid(false),
              m_bHasDrawPosition(false),
              m_cachedDrawPosition(Vec2::Zero()),
              m_cachedPaintMatrix(1.0f),
              m_cachedDrawBounds({}),
              m_cachedOverflowClip(Bounds2D::unconstrained())
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
            return isDisplayable() && isVisible() && isSolid() && hasPrimitive() && !m_bIsCulled;
        }

        bool Component::isFocusable() const
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
            for (Component* ancestor = m_parent; ancestor && ancestor != this; ancestor = ancestor->m_parent)
            {
                if (ancestor->m_styleFile)
                {
                    ancestors.push_back(ancestor->m_styleFile);
                }

                if (ancestor->isRoot())
                {
                    break;
                }
            }

            for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it)
            {
                addFile(*it);
            }

            addFile(m_styleFile);

            if (files.empty())
            {
                m_style.restore();

                return;
            }

            StyleRuleset::Properties properties;
            for (StyleFile* file : files)
            {
                for (const StyleRuleset& source : file->getRulesets())
                {
                    if (source.isEmpty())
                    {
                        continue;
                    }

                    bool bMatched = false;

                    if (!source.compiled.empty())
                    {
                        for (const StyleCompiledSelector& selector : source.compiled)
                        {
                            if (!matchesCompiledSelector(selector))
                            {
                                continue;
                            }

                            bMatched = true;

                            break;
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

                            bMatched = true;

                            break;
                        }
                    }

                    if (!bMatched)
                    {
                        continue;
                    }

                    for (const auto& [key, value] : source.properties)
                    {
                        if (key.startsWith(Style::VARIABLE_KEYWORD))
                        {
                            m_styleVariables[key.substr(1)] = value;

                            continue;
                        }

                        properties[key] = value;
                    }
                }
            }

            if (!properties.empty())
            {
                addStyleProperties(properties);
            }

            m_styleBindingSource = {};
            m_bStyleHasBinding   = false;
            for (const auto& [key, value] : properties)
            {
                if (value.contains("ref(") || isReference(value))
                {
                    m_bStyleHasBinding = true;
                    if (!m_styleBindingSource.isEmpty())
                    {
                        m_styleBindingSource.append('\n');
                    }

                    m_styleBindingSource.append(value);
                }
            }

            m_style.restore();
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            if (
                hasParent() && m_parent->m_bLaidOutThisFrame &&
                !m_style.isPosition(StylePosition::Absolute) && isFlexNowrap(m_parent->getStyle()) &&
                (m_style.isFillPercent(m_style.width.value.getRaw()) ||
                 m_style.isFillPercent(m_style.height.value.getRaw()))
            )
            {
                m_style.resolveFillPercent(m_parent->getRemainingContentSize(this));
            }

            const bool bIsWidthAuto  = m_style.width.isAuto();
            const bool bIsHeightAuto = m_style.height.isAuto();

            float width  = m_style.width.value.get();
            float height = m_style.height.value.get();

            if (bIsWidthAuto || bIsHeightAuto)
            {
                const Vec2 content = getChildrenContentSize();

                if (bIsWidthAuto)
                {
                    const bool bIsFlexRowItem = hasParent() && m_parent->getStyle().isDisplay(StyleDisplay::Flex) &&
                                                m_parent->getStyle().flex.direction.get() == StyleFlexDirection::Row &&
                                                !m_style.isPosition(StylePosition::Absolute);
                    const bool bHasAutoHorizontalMargin =
                        m_style.margin.left.isRaw(Size::AUTO_KEYWORD) || m_style.margin.right.isRaw(Size::AUTO_KEYWORD);

                    if (bIsFlexRowItem || bHasAutoHorizontalMargin || !hasParent())
                    {
                        width = content.x;
                    }
                    else
                    {
                        const Component* box       = getContainingBlock();
                        const float      available = innerLayoutSize(box).x;
                        const float      horizontalMargin =
                            (m_style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.left.get()) +
                            (m_style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.right.get());

                        width = std::max(0.0f, available - horizontalMargin - m_style.insetHorizontal());
                    }
                }

                if (bIsHeightAuto)
                {
                    height = content.y;
                }
            }

            m_style.clampSize(width, height);
            setSize(width, height);
            m_bInsetsApplied = false;
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

                const bool bLeftAuto     = m_style.margin.left.isRaw(Size::AUTO_KEYWORD);
                const bool bRightAuto    = m_style.margin.right.isRaw(Size::AUTO_KEYWORD);
                const bool bTopAuto      = m_style.margin.top.isRaw(Size::AUTO_KEYWORD);
                const bool bBottomAuto   = m_style.margin.bottom.isRaw(Size::AUTO_KEYWORD);
                const bool bParentCenter = parentStyle.align.get() == StyleAlignment::Center;
                const bool bParentFlex   = parentStyle.isDisplay(StyleDisplay::Flex);
                const bool bParentRow    = bParentFlex && parentStyle.flex.direction.get() == StyleFlexDirection::Row;

                float leftoverW = available.x - usedWidth - marginLeft - marginRight;

                if (!m_style.isPosition(StylePosition::Absolute) && bParentRow)
                {
                    leftoverW = m_parent->getPosition().x + parentStyle.insetLeft() + innerLayoutSize(m_parent).x -
                                m_parent->getCursor().x - usedWidth - marginLeft - marginRight;
                }

                if (leftoverW > 0.0f)
                {
                    if (bParentCenter || (bLeftAuto && bRightAuto))
                    {
                        marginLeft  = leftoverW * 0.5f;
                        marginRight = leftoverW * 0.5f;
                    }
                    else if (bLeftAuto)
                    {
                        marginLeft = leftoverW;
                    }
                    else if (bRightAuto)
                    {
                        marginRight = leftoverW;
                    }
                }

                const bool bParentHeightAuto = parentStyle.height.isAuto();

                const bool bCanAutoVertical =
                    m_style.isPosition(StylePosition::Absolute) || bParentFlex || !bParentHeightAuto;

                if (bCanAutoVertical && (bParentCenter || bTopAuto || bBottomAuto))
                {
                    float leftoverH = available.y - usedHeight - marginTop - marginBottom;

                    if (!m_style.isPosition(StylePosition::Absolute) && bParentFlex && !bParentRow)
                    {
                        leftoverH = m_parent->getPosition().y + parentStyle.insetTop() + innerLayoutSize(m_parent).y -
                                    m_parent->getCursor().y - usedHeight - marginTop - marginBottom;
                    }

                    if (leftoverH > 0.0f)
                    {
                        if (bParentCenter || (bTopAuto && bBottomAuto))
                        {
                            marginTop    = leftoverH * 0.5f;
                            marginBottom = leftoverH * 0.5f;
                        }
                        else if (bTopAuto)
                        {
                            marginTop = leftoverH;
                        }
                        else if (bBottomAuto)
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

                const float lineStart    = bIsRow ? (m_parent->getPosition().x + parentStyle.insetLeft())
                                                  : (m_parent->getPosition().y + parentStyle.insetTop());
                const float lineLimit    = bIsRow ? (lineStart + available.x) : (lineStart + available.y);
                float       cursorMain   = bIsRow ? m_parent->getCursor().x : m_parent->getCursor().y;
                const bool  bLineStarted = cursorMain > lineStart;

                if (bLineStarted)
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

        std::vector<Component*> Component::getChildrenFlat() const
        {
            return m_flatChildren;
        }

        bool Component::isRoot() const
        {
            return (!m_parent && !m_root) || (m_parent == this && m_root == this);
        }

        bool Component::isDisplayable() const
        {
            const bool bIsParentDisplyable = isRoot() ? true : m_parent->isDisplayable();
            const bool bIsDisplayable =
                !m_style.isDisplay(StyleDisplay::None) && !m_style.isDisplay(StyleDisplay::Hidden);

            return bIsParentDisplyable && bIsDisplayable;
        }

        bool Component::isVisible() const
        {
            const bool bIsBackgroundImageVisible = !m_style.background.image.getRaw().isEmpty();
            const bool bIsBackgroundColorVisible = m_style.background.color.get().a > 0.0f;
            const bool bIsBackdropVisible        = m_style.backdrop.blur.get() > 0.0f;
            const bool bIsBorderVisible          = m_style.border.isVisible();

            return (bIsBackgroundImageVisible || bIsBackgroundColorVisible || bIsBackdropVisible || bIsBorderVisible) &&
                   getOpacity() > 0.0f;
        }

        bool Component::isSolid() const
        {
            return m_size.x > 0.0f && m_size.y > 0.0f;
        }

        bool Component::isHovered() const
        {
            return m_bIsHovered;
        }

        bool Component::isFocused() const
        {
            return m_bIsFocused;
        }

        bool Component::isDragging() const
        {
            return m_bIsDragging;
        }

        bool Component::isCulled() const
        {
            return m_bIsCulled;
        }

        bool Component::canAdopt(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
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

        void Component::setHovered(bool inValue, bool bInvalidateSubtree)
        {
            if (m_bIsHovered == inValue)
            {
                return;
            }

            m_bIsHovered = inValue;

            if (bInvalidateSubtree)
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

        void Component::setFocused(bool inValue, bool bInvalidateSubtree)
        {
            if (m_bIsFocused == inValue)
            {
                return;
            }

            m_bIsFocused = inValue;

            if (bInvalidateSubtree)
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

        void Component::setDragging(bool inValue, bool bInvalidateSubtree)
        {
            if (m_bIsDragging == inValue)
            {
                return;
            }

            m_bIsDragging = inValue;

            if (bInvalidateSubtree)
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

        void Component::tick(float inDeltaTime)
        {
            m_animationDelta    = inDeltaTime;
            m_bLaidOutThisFrame = false;

            refresh();

            m_animationDelta = 0.0f;

            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            // Off-screen skip is for idle subtrees. A node that just laid out still
            // needs children to measure (text/icons) even if the first cull was stale.
            if (m_bIsCulled && !m_bLaidOutThisFrame && m_animator.isIdle())
            {
                return;
            }

            onTick(inDeltaTime);

            for (std::size_t i = 0; i < m_children.size(); i++)
            {
                m_children.at(i)->tick(inDeltaTime);
            }

            if (!m_bLaidOutThisFrame)
            {
                return;
            }

            // Children may have grown during onRefresh (Text glyphs). Re-measure
            // auto boxes, then re-place children before padding is applied.
            const bool bIsWidthAuto  = isWidthAuto(m_style);
            const bool bIsHeightAuto = isHeightAuto(m_style);

            if (bIsWidthAuto || bIsHeightAuto)
            {
                const Vec2 previous = getContentSize();

                refreshSize();

                if (
                    m_parent && (std::abs(previous.x - getContentSize().x) > 0.01f ||
                                 std::abs(previous.y - getContentSize().y) > 0.01f)
                )
                {
                    m_parent->markLayoutDirty();
                }
            }

            reflowChildPositions();

            if (!m_bInsetsApplied)
            {
                addSize(
                    bIsWidthAuto ? m_style.insetHorizontal() : 0.0f,
                    bIsHeightAuto ? m_style.insetVertical() : 0.0f
                );
                m_bInsetsApplied = true;
            }

            refreshBounds();
            invalidateDrawCacheSubtree();
            refreshCullSubtree();
        }

        void Component::refresh()
        {
            float parentWidth  = 0.0f;
            float parentHeight = 0.0f;
            float parentFont   = 0.0f;
            bool  bParentLaidOut = false;
            if (hasParent() && !isRoot())
            {
                const Vec2 parentContent = m_parent->getContentSize();
                parentWidth    = parentContent.x;
                parentHeight   = parentContent.y;
                parentFont     = m_parent->getStyle().font.size.get();
                bParentLaidOut = m_parent->m_bLaidOutThisFrame;
            }

            const bool bParentSizeChanged = parentWidth != m_layoutParentWidth || parentHeight != m_layoutParentHeight;
            if (bParentSizeChanged)
            {
                m_layoutParentWidth  = parentWidth;
                m_layoutParentHeight = parentHeight;

                // Parent size still includes last frame's post-child insets when that
                // parent skipped layout. Reflowing against that value (and a stale flex
                // cursor) collapses the tree. Only react when the parent actually laid out.
                if (bParentLaidOut || isRoot())
                {
                    m_bIsLayoutDirty = true;
                    m_bIsStyleDirty  = true;
                }
            }

            const bool bParentFontChanged = parentFont != m_layoutParentFontSize;
            if (bParentFontChanged)
            {
                m_layoutParentFontSize = parentFont;

                if (bParentLaidOut || isRoot())
                {
                    m_bIsStyleDirty  = true;
                    m_bIsLayoutDirty = true;
                }
            }

            if (hideIfDirective())
            {
                return;
            }

            if (bParentLaidOut)
            {
                m_bIsLayoutDirty = true;
            }

            if (!m_bIsLayoutDirty && (isWidthAuto(m_style) || isHeightAuto(m_style)))
            {
                for (Component* child : m_children)
                {
                    if (child && child->m_bIsLayoutDirty)
                    {
                        m_bIsLayoutDirty = true;

                        break;
                    }
                }
            }

            if (m_bStyleHasBinding)
            {
                String resolved;
                for (const String& part : m_styleBindingSource.split('\n'))
                {
                    if (part.isEmpty())
                    {
                        continue;
                    }

                    resolved.append(parseText(part.contains("ref(") ? expandStyleBinding(part) : part));
                    resolved.append('\n');
                }

                if (!resolved.equals(m_styleBindingResolved))
                {
                    m_styleBindingResolved = resolved;
                    m_bIsStyleDirty        = true;
                    m_bIsLayoutDirty       = true;
                }
            }

            if (m_bClassHasBinding || m_bIsStyleDirty)
            {
                refreshClassName();
            }

            const bool bStyleRefreshed = m_bIsStyleDirty;

            if (m_bIsStyleDirty)
            {
                const LayoutMetrics before = captureLayoutMetrics(m_style);

                refreshStyleRuleset();
                m_bIsStyleDirty = false;
                refreshStyle();

                if (!(before == captureLayoutMetrics(m_style)))
                {
                    m_bIsLayoutDirty = true;
                }
            }
            else
            {
                const StyleDisplay previousDisplay = m_style.display.get();

                m_style.display.refresh();

                if (previousDisplay != m_style.display.get())
                {
                    m_bIsLayoutDirty = true;
                }
            }

            refreshDirectives();

            if (m_style.isDisplay(StyleDisplay::None))
            {
                m_bIsCulled      = false;
                m_bIsLayoutDirty = false;

                return;
            }

            const bool bAnimating  = !m_animator.isIdle();
            const bool bIsAbsolute = m_style.isPosition(StylePosition::Absolute);
            const bool bFlowLocked = hasParent() && !isRoot() && !bParentLaidOut && !bIsAbsolute;

            if (m_bIsLayoutDirty && bFlowLocked)
            {
                // Parent flex cursor is stale. Keep this node's slot and reflow
                // children inside it so labels can center without collapsing the tree.
                const Vec2 previousContent = getContentSize();
                refreshSize();
                resetFlowCursor();
                refreshBounds();
                invalidateDrawCache();
                m_bIsLayoutDirty    = false;
                m_bLaidOutThisFrame = true;

                for (Component* child : m_children)
                {
                    if (child)
                    {
                        child->markLayoutDirty();
                    }
                }

                if (
                    m_parent && (std::abs(previousContent.x - getContentSize().x) > 0.01f ||
                                 std::abs(previousContent.y - getContentSize().y) > 0.01f)
                )
                {
                    m_parent->markLayoutDirty();
                }
            }
            else if (m_bIsLayoutDirty)
            {
                refreshSize();
                refreshPosition();
                refreshBounds();
                invalidateDrawCache();
                m_bIsLayoutDirty    = false;
                m_bLaidOutThisFrame = true;

                for (Component* child : m_children)
                {
                    if (child)
                    {
                        child->markLayoutDirty();
                    }
                }
            }

            if (m_bLaidOutThisFrame && m_animator.isIdle() && m_style.transform.getRaw().contains('%'))
            {
                m_style.transform.refresh();
                invalidateDrawCache();
            }

            if (bAnimating || bStyleRefreshed || m_bLaidOutThisFrame || !m_bIsAnimationReady)
            {
                tickAnimation(m_style, m_animationDelta);

                if (!m_animator.isIdle())
                {
                    invalidateDrawCacheSubtree();
                }
            }

            onRefresh();

            m_bIsCulled = isCulledByAncestor();
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
            if (m_bSkipForDirective)
            {
                return;
            }

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

        const String& Component::getAttribute(const String& inName) const
        {
            const auto& found = m_attributes.find(inName);

            if (found == m_attributes.end())
            {
                return String::empty();
            }

            return found->second;
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
                child->setStyleFile(child->m_bOwnsStyle ? child->m_styleFile : inSource);
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
            m_bOwnsStyle = true;
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
            float opacity = m_style.opacity.get();

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                opacity *= ancestor->getStyle().opacity.get();

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return opacity;
        }

        float Component::getFilterBlur() const
        {
            float blur = m_style.filter.blur.get();

            const Component* ancestor = m_parent;
            while (ancestor)
            {
                const float parentBlur = ancestor->getStyle().filter.blur.get();
                if (parentBlur > 0.0f)
                {
                    blur = std::sqrt((blur * blur) + (parentBlur * parentBlur));
                }

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return blur;
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

            String value  = inValue.trim();
            bool   bHover = false;
            bool   bFocus = false;
            bool   bDrag  = false;

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

                consider(hoverAt, Style::PSEUDO_CLASS_HOVER, bHover);
                consider(focusAt, Style::PSEUDO_CLASS_FOCUS, bFocus);
                consider(dragAt, Style::PSEUDO_CLASS_DRAG, bDrag);

                if (!token)
                {
                    break;
                }

                *flag = true;
                value = value.substr(0, at) + value.substr(at + std::strlen(token));
            }

            if (bHover && !m_bIsHovered)
            {
                return false;
            }

            if (bFocus && !m_bIsFocused)
            {
                return false;
            }

            if (bDrag && !m_bIsDragging)
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

                for (std::size_t i = 1; i < value.size(); ++i)
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

            if (!hasParent())
            {
                return false;
            }

            Component* ancestor = getParent();

            int index = static_cast<int>(parts.size()) - 2;
            while (ancestor && index >= 0)
            {
                if (ancestor->hasLocalSelector(parts[index].trim()))
                {
                    index--;
                }

                ancestor = ancestor->getParent();
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

            if (!hasParent())
            {
                return false;
            }

            Component* ancestor = getParent();
            int        index    = static_cast<int>(inSelector.chain.size()) - 2;

            while (ancestor && index >= 0)
            {
                if (ancestor->matchesCompiledPart(inSelector.chain[static_cast<std::size_t>(index)]))
                {
                    index--;
                }

                ancestor = ancestor->getParent();
            }

            return index < 0;
        }

        bool Component::matchesCompiledPart(const StyleSelectorPart& inPart) const
        {
            if (inPart.bCanHover && !m_bIsHovered)
            {
                return false;
            }

            if (inPart.bCanFocus && !m_bIsFocused)
            {
                return false;
            }

            if (inPart.bCanDrag && !m_bIsDragging)
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

        void Component::markFlatDirty()
        {
            rebuildFlatChildren();

            for (Component* ancestor = m_parent; ancestor && ancestor != this; ancestor = ancestor->m_parent)
            {
                ancestor->rebuildFlatChildren();

                if (ancestor->isRoot())
                {
                    break;
                }
            }
        }

        void Component::markStyleDirty()
        {
            m_bIsStyleDirty = true;
        }

        void Component::markLayoutDirty()
        {
            m_bIsLayoutDirty = true;
            invalidateDrawCache();
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

        void Component::invalidateDrawCache()
        {
            m_bDrawCacheValid  = false;
            m_bHasDrawPosition = false;
        }

        void Component::invalidateDrawCacheSubtree()
        {
            invalidateDrawCache();

            for (Component* child : m_children)
            {
                if (child)
                {
                    child->invalidateDrawCacheSubtree();
                }
            }
        }

        Vec2 Component::getScrollOffset() const
        {
            return Vec2::Zero();
        }

        std::uint64_t Component::getScrollGeneration() const
        {
            return 0;
        }

        void Component::cacheAttributeFlags()
        {
            m_bClassHasBinding = isReference(getAttribute(CLASS_ATTRIBUTE_NAME));
            m_bHasIfDirective  = !getAttribute(IF_DIRECTIVE_KEYWORD).isEmpty();
        }

        bool Component::hideIfDirective()
        {
            if (!m_bHasIfDirective)
            {
                return false;
            }

            const String& attribute = getAttribute(IF_DIRECTIVE_KEYWORD);
            if (attribute.isEmpty())
            {
                return false;
            }

            const bool bShow = parseText(attribute).equals("true", "1");
            if (bShow)
            {
                if (m_style.isDisplay(StyleDisplay::None))
                {
                    m_bIsStyleDirty  = true;
                    m_bIsLayoutDirty = true;
                }

                return false;
            }

            m_style.display.set(StyleDisplay::None);
            m_bIsCulled      = false;
            m_bIsStyleDirty  = false;
            m_bIsLayoutDirty = false;

            return true;
        }

        Component* Component::getHitAt(const Vec2& inLocation) const
        {
            Component* hit = nullptr;

            for (Component* child : getChildrenFlat())
            {
                // Cheapest rejection first: everything below walks ancestors or builds a paint matrix.
                if (child->isCulled())
                {
                    continue;
                }

                if (!child->isDisplayable())
                {
                    continue;
                }

                if (!child->getDrawBounds().contains(inLocation))
                {
                    continue;
                }

                if (!child->containsPoint(inLocation))
                {
                    continue;
                }

                if (!hit || child->getDepth() >= hit->getDepth())
                {
                    hit = child;
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

            Vec2       local   = inLocation;
            const Mat3 paint   = getPaintMatrix();
            const Mat3 inverse = glm::inverse(static_cast<glm::mat3>(paint));
            const Vec3 mapped  = inverse * Vec3(inLocation.x, inLocation.y, 1.0f);

            local.x = mapped.x;
            local.y = mapped.y;

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

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return true;
        }

        bool Component::broadcastEvent(const WindowEvent& inEvent)
        {
            for (Component* child : getChildrenFlat())
            {
                if (child->onEvent(inEvent))
                {
                    return true;
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

        void Component::addChildren(const pugi::xml_node& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            bool bAdopted = false;
            for (const auto& child : inNode.children())
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
                    bAdopted = true;
                }
            }

            if (bAdopted)
            {
                markFlatDirty();
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
            inComponent->setStyleFile(inComponent->m_bOwnsStyle ? inComponent->m_styleFile : m_styleFile);

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

            markFlatDirty();
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
            markFlatDirty();
            markLayoutDirty();
        }

        Vec2 Component::getChildrenContentSizeBlock() const
        {
            return getChildrenContentSizeFromOrigin(
                getPosition().x + m_style.insetLeft(),
                getPosition().y + m_style.insetTop()
            );
        }

        Vec2 Component::getChildrenContentSizeFlex() const
        {
            const bool  bIsRow     = m_style.flex.direction.get() == StyleFlexDirection::Row;
            const bool  bCanWrap   = m_style.flex.wrap.get() == StyleFlexWrap::Wrap;
            const float mainGap    = bIsRow ? m_style.gap.left.get() : m_style.gap.top.get();
            const float crossGap   = bIsRow ? m_style.gap.top.get() : m_style.gap.left.get();
            const Vec2  inner      = innerLayoutSize(this);
            const float innerMain  = bIsRow ? inner.x : inner.y;

            float lineMain     = 0.0f;
            float lineCross    = 0.0f;
            float totalMain    = 0.0f;
            float totalCross   = 0.0f;
            bool  bLineStarted = false;

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style = child->getStyle();
                const Vec2   size  = getChildIntrinsicSize(child);
                const float  marginLeft =
                    style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get();
                const float  marginRight =
                    style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get();
                const float  marginTop =
                    style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get();
                const float  marginBottom =
                    style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get();

                const float itemMain  = bIsRow ? (marginLeft + size.x + marginRight)
                                               : (marginTop + size.y + marginBottom);
                const float itemCross = bIsRow ? (marginTop + size.y + marginBottom)
                                               : (marginLeft + size.x + marginRight);

                if (bCanWrap && bLineStarted && innerMain > 0.0f && (lineMain + mainGap + itemMain) > innerMain)
                {
                    totalMain    = std::max(totalMain, lineMain);
                    totalCross   = totalCross > 0.0f ? totalCross + crossGap + lineCross : lineCross;
                    lineMain     = 0.0f;
                    lineCross    = 0.0f;
                    bLineStarted = false;
                }

                if (bLineStarted)
                {
                    lineMain += mainGap;
                }

                lineMain += itemMain;
                lineCross      = std::max(lineCross, itemCross);
                bLineStarted = true;
            }

            totalMain = std::max(totalMain, lineMain);
            if (bLineStarted)
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

            const bool bWidthAuto  = isWidthAuto(style);
            const bool bHeightAuto = isHeightAuto(style);

            if (bWidthAuto || bHeightAuto)
            {
                const Vec2 inner = inChild->getChildrenContentSize();

                if (bWidthAuto)
                {
                    size.x = std::max(size.x, inner.x);
                }

                if (bHeightAuto)
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
            if (isRoot())
            {
                return 0.0f;
            }

            return m_parent->getDepth() + m_style.zIndex.get() + 0.1f;
        }

        const Vec2& Component::getSize() const
        {
            return m_size;
        }

        Vec2 Component::getContentSize() const
        {
            Vec2 size = m_size;

            if (!isWidthAuto(m_style) || m_bInsetsApplied)
            {
                size.x = std::max(0.0f, size.x - m_style.insetHorizontal());
            }

            if (!isHeightAuto(m_style) || m_bInsetsApplied)
            {
                size.y = std::max(0.0f, size.y - m_style.insetVertical());
            }

            return size;
        }

        Vec2 Component::getBorderSize() const
        {
            Vec2 size = m_size;

            if (isWidthAuto(m_style) && !m_bInsetsApplied)
            {
                size.x += m_style.insetHorizontal();
            }

            if (isHeightAuto(m_style) && !m_bInsetsApplied)
            {
                size.y += m_style.insetVertical();
            }

            return size;
        }

        Vec2 Component::getRemainingContentSize() const
        {
            return getRemainingContentSize(nullptr);
        }

        Vec2 Component::getRemainingContentSize(const Component* inChild) const
        {
            const Vec2 inner = getContentSize();

            if (!inChild || !isFlexNowrap(m_style))
            {
                return inner;
            }

            const bool  bRow      = m_style.flex.direction.get() == StyleFlexDirection::Row;
            const float mainGap   = bRow ? m_style.gap.left.get() : m_style.gap.top.get();
            const float innerMain = bRow ? inner.x : inner.y;

            float used    = 0.0f;
            bool  bStarted = false;

            for (const Component* sibling : m_children)
            {
                if (!sibling || !sibling->isDisplayable() || sibling->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                if (sibling == inChild)
                {
                    break;
                }

                const Style& style = sibling->getStyle();
                const Vec2   box   = sibling->getBorderSize();
                const float  marginStart =
                    bRow ? (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get())
                         : (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get());
                const float marginEnd =
                    bRow ? (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get())
                         : (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get());
                const float itemMain = marginStart + (bRow ? box.x : box.y) + marginEnd;

                if (bStarted)
                {
                    used += mainGap;
                }

                used += itemMain;
                bStarted = true;
            }

            const float remainingMain = std::max(0.0f, innerMain - used);

            return bRow ? Vec2(remainingMain, inner.y) : Vec2(inner.x, remainingMain);
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
            invalidateDrawCacheSubtree();
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

            m_size.x         = width;
            m_size.y         = height;
            m_bInsetsApplied = false;
            invalidateDrawCache();
            m_bIsLayoutDirty = true;
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
            if (!m_bHasDrawPosition)
            {
                m_cachedDrawPosition = computeDrawPosition();
                m_bHasDrawPosition   = true;
            }

            return m_cachedDrawPosition;
        }

        Vec2 Component::computeDrawPosition() const
        {
            Vec2 result = getTranslation();

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                const Vec2 scroll = ancestor->getScrollOffset();
                result.x -= scroll.x;
                result.y -= scroll.y;

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return result;
        }

        Vec2 Component::getTransformPivot() const
        {
            return getDrawPosition() + getStyle().getTransformOrigin(getBorderSize());
        }

        Mat3 Component::getPaintMatrix() const
        {
            if (!m_bDrawCacheValid)
            {
                m_cachedPaintMatrix = computePaintMatrix();
            }

            return m_cachedPaintMatrix;
        }

        Mat3 Component::computePaintMatrix() const
        {
            const Component* chain[64];
            int              count = 0;
            const Component* node  = this;

            while (node && count < 64)
            {
                chain[count++] = node;

                if (node->isRoot() || !node->hasParent())
                {
                    break;
                }

                node = node->getParent();
            }

            Mat3 world(1.0f);

            for (int i = count - 1; i >= 0; --i)
            {
                const Component*     component = chain[i];
                const StyleTransform xform     = component->getStyle().getTransform();

                if (xform.isIdentity())
                {
                    continue;
                }

                const Vec2  origin  = component->getTransformPivot();
                const float radians = glm::radians(xform.rotation);
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
                    origin.x + xform.translation.x,
                    origin.y + xform.translation.y,
                    1.0f
                );
                const Mat3 linear(
                    cosine * xform.scale.x,
                    sine * xform.scale.x,
                    0.0f,
                    -sine * xform.scale.y,
                    cosine * xform.scale.y,
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f
                );

                world = world * fromOrigin * linear * toOrigin;
            }

            return world;
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

        Bounds2D Component::getDrawBounds() const
        {
            if (!m_bDrawCacheValid)
            {
                Bounds2D result;

                const Vec2 position = getDrawPosition();

                result.left   = position.x;
                result.top    = position.y;
                result.right  = position.x + m_size.x;
                result.bottom = position.y + m_size.y;

                const Mat3 paint = getPaintMatrix();

                result.set(result.top, result.left, result.bottom, result.right);
                result.transform(paint);

                m_cachedDrawBounds    = result;
                m_cachedOverflowClip  = Bounds2D::unconstrained();

                const Component* ancestor = m_parent;
                while (ancestor && ancestor != this)
                {
                    if (ancestor->getStyle().isClippingOverflow())
                    {
                        m_cachedOverflowClip = m_cachedOverflowClip.intersect(ancestor->getDrawBounds());
                    }

                    if (ancestor->isRoot())
                    {
                        break;
                    }

                    ancestor = ancestor->getParent();
                }

                m_bDrawCacheValid = true;
            }

            return m_cachedDrawBounds;
        }

        Bounds2D Component::getOverflowClip() const
        {
            if (!m_bDrawCacheValid)
            {
                getDrawBounds();
            }

            return m_cachedOverflowClip;
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
            const Vec4 unconstrained(-1.0e9f, -1.0e9f, 1.0e9f, 1.0e9f);

            outFirst         = unconstrained;
            outFirstRadiusX  = Vec4::Zero();
            outFirstRadiusY  = Vec4::Zero();
            outSecond        = unconstrained;
            outSecondRadiusX = Vec4::Zero();
            outSecondRadiusY = Vec4::Zero();

            std::uint32_t filled = 0;

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                if (ancestor->getStyle().isClippingOverflow() && !ancestor->getStyle().radius.isZero())
                {
                    const Bounds2D box     = ancestor->getDrawBounds();
                    const Vec4     radiusX = ancestor->getStyle().radius.horizontal();
                    const Vec4     radiusY = ancestor->getStyle().radius.vertical();
                    const Vec4     clip    = Vec4(box.left, box.top, box.right, box.bottom);

                    if (filled == 0)
                    {
                        outFirst        = clip;
                        outFirstRadiusX = radiusX;
                        outFirstRadiusY = radiusY;
                    }
                    else
                    {
                        outSecond        = clip;
                        outSecondRadiusX = radiusX;
                        outSecondRadiusY = radiusY;

                        break;
                    }

                    filled++;
                }

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }
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

            pugi::xml_document document = Xml::load(inTemplate);
            if (document.empty() || document.children().empty())
            {
                throw std::runtime_error("UI document " + source + " does not have any components");
            }

            const pugi::xml_node root = document.first_child();
            if (!(root.parent() == root.root() && !root.next_sibling()))
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            std::vector<Component*> projected = m_children;
            m_children.clear();

            Scope      scope(this);
            Projection projection(projected);

            if (String(root.name()).equals(getTag()))
            {
                m_sourceNode = m_sourceDocument.append_copy(root);
                m_attributes = Xml::getAttributes(m_sourceNode);
                cacheAttributeFlags();
                setId(getAttribute(ID_ATTRIBUTE_NAME));
                setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));
                addChildren(root);
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

        void Component::addProjectedContent(const pugi::xml_node& inSlot)
        {
            if (!g_projected || g_projected->empty())
            {
                return;
            }

            const pugi::xml_attribute attribute = Xml::getAttribute(CONTENT_SELECT_ATTRIBUTE_NAME, inSlot);
            const String select = attribute.empty() ? String::empty() : String(attribute.as_string()).trim();

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
                child->invalidateDrawCache();
                child->reflowChildPositions();
            }
        }

        void Component::refreshCullSubtree()
        {
            if (m_style.isDisplay(StyleDisplay::None))
            {
                m_bIsCulled = false;

                return;
            }

            m_bIsCulled = isCulledByAncestor();

            for (Component* child : m_children)
            {
                if (child)
                {
                    child->refreshCullSubtree();
                }
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

            for (auto iterator = ancestors.rbegin(); iterator != ancestors.rend(); ++iterator)
            {
                addFile(*iterator);
            }

            addFile(m_styleFile);

            if (m_bOwnsStyle)
            {
                addFile(m_styles.get());
            }

            for (auto iterator = files.rbegin(); iterator != files.rend(); ++iterator)
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
            clip.duration   = m_style.animation.duration;
            clip.iterations = m_style.animation.iterations;
            clip.loop       = m_style.animation.bAlternate
                                  ? Drift::Loop::PingPong
                                  : (m_style.animation.iterations == 1 ? Drift::Loop::Once : Drift::Loop::Repeat);

            std::unordered_map<String, Drift::Track> tracks;

            for (const StyleKeyframe& frame : inKeyframes)
            {
                Style snapshot;
                snapshot.setParent(this);
                snapshot.setProperties(frame.properties);
                snapshot.refresh();

                const float time = frame.offset * clip.duration;

                for (const auto& [name, value] : frame.properties)
                {
                    const std::vector<float> parsed = snapshot.extractAnimatedProperty(name);

                    if (parsed.empty())
                    {
                        continue;
                    }

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
                }

                if (type && instance)
                {
                    if (const ReflectionTypeMethodInfo* method = type->findMethod(name))
                    {
                        return method->toString(method->invoke(instance));
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
            if (m_sourceNode.empty())
            {
                return nullptr;
            }

            Scope      scope(m_importOwner);
            Component* clone = create(m_sourceNode);
            if (!clone)
            {
                return nullptr;
            }

            clone->m_bSkipForDirective = true;
            clone->m_attributes.erase(FOR_DIRECTIVE_KEYWORD);

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

            bool bMutated = false;

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
                bMutated = true;
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

                bMutated = true;
            }

            for (std::size_t i = 0; i < m_forInstances.size(); ++i)
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
                    bMutated = true;
                }
            }

            if (bMutated)
            {
                parent->markLayoutDirty();
                parent->markFlatDirty();
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
                if (!accessor.isValid() && hasParent())
                {
                    accessor = m_parent->getField(param);
                }

                if (accessor.isValid())
                {
                    const void* instance =
                        accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(this);

                    if (expected.endsWith('*'))
                    {
                        outMethod.addParam(const_cast<void*>(static_cast<const void*>(accessor.address(instance))));

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

        void Component::rebuildFlatChildren()
        {
            m_flatChildren.clear();
            m_flatChildren.reserve(m_children.size() * 2);

            for (Component* child : m_children)
            {
                if (!child || child == this)
                {
                    continue;
                }

                m_flatChildren.push_back(child);

                const std::vector<Component*> sub = child->getChildrenFlat();
                m_flatChildren.insert(m_flatChildren.end(), sub.begin(), sub.end());
            }
        }
    }
}