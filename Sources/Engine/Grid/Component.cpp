#include "Chicane/Grid/Component.hpp"

#include <algorithm>

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            m_attributes = Xml::getAttributes(inNode);
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            addChildren(inNode);
        }

        Component::Component(const String& inTag)
            : m_tag(inTag),
              m_style({}),
              m_styleFile(nullptr),
              m_functions({}),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_size(Vec2::Zero),
              m_position(Vec2::Zero),
              m_cursor(Vec2::Zero),
              m_bounds({}),
              m_attributes({}),
              m_primitive({})
        {
            m_style.setParent(this);
        }

        Component::~Component()
        {
            for (Component* child : m_children)
            {
                delete child;
                child = nullptr;
            }

            m_children.clear();
        }

        bool Component::isDrawable() const
        {
            return isDisplayable() && isVisible() && isSolid() && hasPrimitive();
        }

        void Component::tick(float inDeltaTime)
        {
            refresh();

            onTick(inDeltaTime);

            for (Component* child : m_children)
            {
                child->tick(inDeltaTime);
            }
        }

        void Component::refresh()
        {
            refreshStyle();
            refreshSize();
            refreshPosition();
            refreshBounds();

            onRefresh();
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

            return (bIsBackgroundImageVisible || bIsBackgroundColorVisible) && m_style.opacity.get() > 0.0f;
        }

        bool Component::isSolid() const
        {
            return m_size.x > 0.0f && m_size.y > 0.0f;
        }

        bool Component::canAdopt(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
        }

        void Component::hover()
        {
            onHover();

            String onHoverAttribute = getAttribute(ON_HOVER_ATTRIBUTE_NAME);

            if (onHoverAttribute.isEmpty())
            {
                return;
            }

            String functionName = parseText(onHoverAttribute);

            if (!hasFunction(functionName))
            {
                return;
            }

            Event event;
            getFunction(functionName)(event);
        }

        void Component::click()
        {
            onClick();

            String onClickAttribute = getAttribute(ON_CLICK_ATTRIBUTE_NAME);

            if (onClickAttribute.isEmpty())
            {
                return;
            }

            String functionName = parseText(onClickAttribute);

            if (!hasFunction(functionName))
            {
                return;
            }

            Event event;
            getFunction(functionName)(event);
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
            m_id = inValue;

            refreshStyleRuleset();
        }

        std::vector<String> Component::getClassList() const
        {
            return m_className.split(Style::SELECTOR_SEPARATOR_SPACE);
        }

        const String& Component::getClassName() const
        {
            return m_className;
        }

        void Component::setClassName(const String& inValue)
        {
            m_className = inValue;

            refreshStyleRuleset();
        }

        String Component::getAttribute(const String& inName) const
        {
            if (m_attributes.find(inName) == m_attributes.end())
            {
                return "";
            }

            return m_attributes.at(inName);
        }

        const Style& Component::getStyle() const
        {
            return m_style;
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
                child->setStyleFile(inSource);
            }
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

        bool Component::hasReference(const String& inId, bool isLocalOnly) const
        {
            bool bWasFoundLocally = m_references.find(inId) != m_references.end() && m_references.at(inId) &&
                                    !m_references.at(inId)->isEmpty();

            if (isRoot() || !hasParent() || isLocalOnly)
            {
                return bWasFoundLocally;
            }

            return bWasFoundLocally || m_parent->hasReference(inId);
        }

        Reference* Component::getReference(const String& inId) const
        {
            if (!hasParent() || isRoot())
            {
                return hasReference(inId, true) ? m_references.at(inId) : nullptr;
            }

            return hasReference(inId, true) ? m_references.at(inId) : m_parent->getReference(inId);
        }

        void Component::addReference(const Reference::Map& inReferences)
        {
            for (auto [id, reference] : inReferences)
            {
                addReference(id, reference);
            }
        }

        void Component::addReference(const String& inId, Reference* inReference)
        {
            if (hasReference(inId, true))
            {
                return;
            }

            m_references.insert(std::make_pair(inId, inReference));
        }

        void Component::removeReference(const String& inId)
        {
            if (!hasReference(inId, true))
            {
                return;
            }

            m_references.erase(inId);
        }

        bool Component::hasFunction(const String& inId, bool isLocalOnly) const
        {
            if (inId.isEmpty())
            {
                return false;
            }

            const String id = inId.split(FUNCTION_PARAMS_OPENING).front().trim();

            const bool bHasLocally =
                m_functions.find(id) != m_functions.end() && m_functions.at(id) && m_functions.at(id) != nullptr;

            if (!hasParent() || isRoot() || isLocalOnly)
            {
                return bHasLocally;
            }

            return bHasLocally || m_parent->hasFunction(id);
        }

        const Function Component::getFunction(const String& inId, bool isLocalOnly) const
        {
            if (inId.isEmpty())
            {
                return nullptr;
            }

            const String id = inId.split(FUNCTION_PARAMS_OPENING).front().trim();

            if (!hasParent() || isRoot() || isLocalOnly)
            {
                return hasFunction(id, true) ? m_functions.at(id) : nullptr;
            }

            return hasFunction(id, true) ? m_functions.at(id) : m_parent->getFunction(id);
        }

        void Component::addFunction(const Functions& inFunctions)
        {
            for (auto [id, function] : inFunctions)
            {
                addFunction(id, function);
            }
        }

        void Component::addFunction(const String& inId, Function inFunction)
        {
            if (hasFunction(inId, true))
            {
                return;
            }

            m_functions.insert(std::make_pair(inId, inFunction));
        }

        void Component::removeFunction(const String& inId)
        {
            if (!hasFunction(inId, true))
            {
                return;
            }

            m_functions.erase(inId);
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
            return m_parent != nullptr;
        }

        Component* Component::getParent() const
        {
            return m_parent;
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

        bool Component::hasNeighbours() const
        {
            if (isRoot())
            {
                return false;
            }

            std::vector<const Component*> neighbours = {};

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

            std::uint32_t location = std::find_if(
                                         neighbours.begin(),
                                         neighbours.end(),
                                         [&](Component* children) { return children == this; }
                                     ) -
                                     neighbours.begin();

            return neighbours.at(std::clamp(location + inJumps, 0U, static_cast<std::uint32_t>(neighbours.size() - 1)));
        }

        bool Component::hasChildren() const
        {
            return !m_children.empty();
        }

        const std::vector<Component*>& Component::getChildren() const
        {
            return m_children;
        }

        std::vector<Component*> Component::getChildrenFlat() const
        {
            std::vector<Component*> result;

            for (Component* child : m_children)
            {
                if (!child)
                {
                    continue;
                }

                result.push_back(child);

                std::vector<Component*> sub = child->getChildrenFlat();
                result.insert(result.end(), sub.begin(), sub.end());
            }

            return result;
        }

        void Component::addChildren(const pugi::xml_node& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            for (const auto& child : inNode.children())
            {
                addChild(createComponent(child));
            }
        }

        void Component::addChild(Component* inComponent)
        {
            if (!canAdopt(inComponent))
            {
                return;
            }

            inComponent->setRoot(m_root);
            inComponent->setParent(this);
            inComponent->setStyleFile(m_styleFile);

            m_children.push_back(inComponent);

            onAdopted(inComponent);
        }

        Vec2 Component::getChildrenContentSizeBlock() const
        {
            Vec2 result = Vec2::Zero;

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable())
                {
                    continue;
                }

                const Style& style = child->getStyle();

                const Vec2 margin = {
                    style.margin.left.get() + style.margin.right.get(),
                    style.margin.top.get() + style.margin.bottom.get()
                };

                const Vec2 occupied = {
                    (child->getPosition().x - m_position.x) + child->getSize().x + margin.x,
                    (child->getPosition().y - m_position.y) + child->getSize().y + margin.y
                };

                result.x = std::max(result.x, occupied.x);
                result.y = std::max(result.y, occupied.y);
            }

            return result;
        }

        Vec2 Component::getChildrenContentSizeFlex() const
        {
            Vec2 result = Vec2::Zero;

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable())
                {
                    continue;
                }

                const Style& style = child->getStyle();

                const Vec2 margin = {style.margin.right.get(), style.margin.bottom.get()};

                result.x = std::max(result.x, (child->getPosition().x - m_position.x) + child->getSize().x + margin.x);
                result.y = std::max(result.y, (child->getPosition().y - m_position.y) + child->getSize().y + margin.y);
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

        const Vec2& Component::getSize() const
        {
            return m_size;
        }

        void Component::setSize(const Vec2& inValue)
        {
            setSize(inValue.x, inValue.y);
        }

        void Component::setSize(float inWidth, float inHeight)
        {
            m_size.x = inWidth;
            m_size.y = inHeight;
        }

        const Vec2& Component::getPosition() const
        {
            return m_position;
        }

        void Component::addPosition(const Vec2& inValue)
        {
            addPosition(inValue.x, inValue.y);
        }

        void Component::addPosition(float inX, float inY)
        {
            setPosition(m_position.x + inX, m_position.y + inY);
        }

        void Component::setPosition(const Vec2& inValue)
        {
            setPosition(inValue.x, inValue.y);
        }

        void Component::setPosition(float inX, float inY)
        {
            m_position.x = inX;
            m_position.y = inY;

            setCursor(m_position);
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
            return m_bounds;
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
            m_primitive = {};
        }

        void Component::setPrimitive(const Primitive& inPrimitive)
        {
            m_primitive = inPrimitive;
        }

        void Component::refreshStyle()
        {
            m_style.refresh();
        }

        void Component::refreshStyleRuleset()
        {
            if (!m_styleFile)
            {
                return;
            }

            const String& tag = getTag();

            String id = Style::ID_SELECTOR;
            id.append(getId());

            std::vector<String> classList;
            String              formattedClassName = "";
            for (const String& className : getClassList())
            {
                String formatted = Style::CLASS_SELECTOR;
                formatted.append(className);

                formattedClassName.append(formatted);
                formattedClassName.append(" ");

                classList.push_back(formatted);
            }
            classList.push_back(formattedClassName.trim());

            for (const StyleRuleset& source : m_styleFile->getRulesets())
            {
                if (source.isEmpty())
                {
                    continue;
                }

                for (const String& selector : source.selectors)
                {
                    if (selector.equals(Style::INCLUSIVE_SELECTOR))
                    {
                        addStyleProperties(source.properties);

                        continue;
                    }

                    if (!tag.isEmpty())
                    {
                        if (selector.equals(tag))
                        {
                            addStyleProperties(source.properties);

                            continue;
                        }
                    }

                    if (!id.isEmpty())
                    {
                        if (id.equals(selector))
                        {
                            addStyleProperties(source.properties);

                            continue;
                        }
                    }

                    if (!classList.empty())
                    {
                        for (const String& className : classList)
                        {
                            if (className.equals(selector))
                            {
                                addStyleProperties(source.properties);

                                continue;
                            }
                        }
                    }
                }
            }
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            const bool bIsWidthAuto  = m_style.width.isRaw(Size::AUTO_KEYWORD);
            const bool bIsHeightAuto = m_style.height.isRaw(Size::AUTO_KEYWORD);

            if (!bIsWidthAuto && !bIsHeightAuto)
            {
                setSize(m_style.width.get(), m_style.height.get());

                return;
            }

            const Vec2 content = getChildrenContentSize();

            setSize(bIsWidthAuto ? content.x : m_style.width.get(), bIsHeightAuto ? content.y : m_style.height.get());
        }

        void Component::refreshPosition()
        {
            setPosition(0.0f, 0.0f);

            Vec2 margin = Vec2(
                m_style.margin.left.get() == m_style.margin.right.get()
                    ? m_style.margin.left.get()
                    : (m_style.margin.left.get() - m_style.margin.right.get()),
                m_style.margin.top.get() == m_style.margin.bottom.get()
                    ? m_style.margin.top.get()
                    : (m_style.margin.top.get() - m_style.margin.bottom.get())
            );
            Vec2 padding = Vec2(
                m_style.padding.left.get() == m_style.padding.right.get()
                    ? m_style.padding.left.get()
                    : (m_style.padding.left.get() - m_style.padding.right.get()),
                m_style.padding.top.get() == m_style.padding.bottom.get()
                    ? m_style.padding.top.get()
                    : (m_style.padding.top.get() - m_style.padding.bottom.get())
            );

            if (isRoot() || m_style.isPosition(StylePosition::Absolute))
            {
                setPosition(margin);
                addCursor(padding);

                return;
            }

            const Style& parentStyle  = m_parent->getStyle();
            const Vec2&  parentCursor = m_parent->getCursor();

            const Vec2 addedSpacing  = margin;
            const Vec2 occupiedSpace = m_size + margin;

            setPosition(parentCursor + addedSpacing);
            addCursor(padding);

            switch (parentStyle.display.get())
            {
            case StyleDisplay::Flex:
                if (parentStyle.flex.direction == StyleFlexDirection::Row)
                {
                    m_parent->addCursor(parentStyle.gap.left.get() + occupiedSpace.x, 0.0f);
                }

                if (parentStyle.flex.direction == StyleFlexDirection::Column)
                {
                    m_parent->addCursor(0.0f, parentStyle.gap.top.get() + occupiedSpace.y);
                }

                break;

            default:
                m_parent->addCursor(0.0f, occupiedSpace.y);

                break;
            }
        }

        void Component::refreshBounds()
        {
            m_bounds.top    = m_position.y;
            m_bounds.bottom = m_bounds.top + m_size.y;
            m_bounds.left   = m_position.x;
            m_bounds.right  = m_bounds.left + m_size.x;
        }

        String Component::parseText(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return "";
            }

            if (!isReference(inValue))
            {
                return inValue;
            }

            const std::uint32_t start = inValue.firstOf(REFERENCE_VALUE_OPENING) + 1;
            const std::uint32_t end   = inValue.lastOf(REFERENCE_VALUE_CLOSING) - 1;

            const String prefix = inValue.substr(0, start - 1);
            const String value  = inValue.substr(start + 1, end - start - 1).trim();
            const String suffix = inValue.substr(end + 2);

            String result = "";

            if (!prefix.isEmpty())
            {
                result.append(parseText(prefix));
            }

            if (!value.isEmpty())
            {
                result.append(parseReference(value).toString());
            }

            if (!suffix.isEmpty())
            {
                result.append(parseText(suffix));
            }

            return result;
        }

        bool Component::isReference(const String& inValue) const
        {
            const bool bHasOpening = inValue.firstOf(REFERENCE_VALUE_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(REFERENCE_VALUE_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        Reference Component::parseReference(const String& inValue) const
        {
            if (!isFunction(inValue))
            {
                if (hasReference(inValue))
                {
                    return *getReference(inValue);
                }

                return Reference::fromValue<const String>(&inValue);
            }

            if (!hasFunction(inValue))
            {
                return Reference::fromValue<const String>(&inValue);
            }

            FunctionData data = parseFunction(inValue);

            Event event  = {};
            event.values = data.params;

            return getFunction(data.name)(event);
        }

        bool Component::isFunction(const String& inValue) const
        {
            const bool bHasOpening = inValue.firstOf(FUNCTION_PARAMS_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(FUNCTION_PARAMS_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        FunctionData Component::parseFunction(const String& inRefValue) const
        {
            const String trimmedValue = inRefValue.trim();

            if (trimmedValue.isEmpty())
            {
                return {};
            }

            const String params = inRefValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING);

            FunctionData data = {};
            data.name         = trimmedValue.substr(0, inRefValue.firstOf(FUNCTION_PARAMS_OPENING) + 1);

            for (const String& value : params.split(FUNCTION_PARAMS_SEPARATOR))
            {
                data.params.push_back(parseReference(value.trim()));
            }

            return data;
        }
    }
}