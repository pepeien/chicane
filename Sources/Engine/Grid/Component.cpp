#include "Chicane/Grid/Component.hpp"

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

                    // In order for this to work I a reflection system is needed
                }
            );
        }

        Component::Component(const String& inTag)
            : m_tag(inTag),
              m_id(String::empty()),
              m_className(String::empty()),
              m_directives({}),
              m_style({}),
              m_styleFile(nullptr),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_size(Vec2::Zero),
              m_scale(Vec2::Zero),
              m_offset(Vec2::Zero),
              m_position(Vec2::Zero),
              m_cursor(Vec2::Zero),
              m_bounds({}),
              m_primitive({}),
              m_attributes({})
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

            addSize(m_style.padding.right.get(), m_style.padding.bottom.get());
        }

        void Component::refresh()
        {
            refreshStyle();

            onRefresh();

            refreshSize();
            refreshPosition();
            refreshBounds();

            refreshDirectives();
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
            m_className = inValue;

            refreshStyleRuleset();
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

        const String& Component::getAttribute(const String& inName) const
        {
            const auto& found = m_attributes.find(inName);

            if (found == m_attributes.end())
            {
                return String::empty();
            }

            return found->second;
        }

        const Style& Component::getStyle() const
        {
            return m_style;
        }

        bool Component::hasLocalSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            if (inValue.equals(Style::INCLUSIVE_SELECTOR))
            {
                return true;
            }

            String              tag;
            String              id;
            std::vector<String> classes;

            String value = inValue.trim();

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
            if (inId.isEmpty())
            {
                return false;
            }

            if (isLocalOnly || !hasParent())
            {
                if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::get().find(typeid(*this)))
                {
                    const std::vector<String> fields = inId.split('.');

                    if (fields.empty())
                    {
                        return false;
                    }

                    return type->findField(fields.at(0)) != nullptr;
                }

                return false;
            }

            return m_parent->hasReference(inId, false);
        }

        bool Component::hasFunction(const String& inId, bool isLocalOnly) const
        {
            return false;

            /*
            if (inId.isEmpty())
            {
                return false;
            }

            const String id = inId.split(FUNCTION_PARAMS_OPENING).front().trim();

            const bool bHasLocally =
                m_functions.find(id) != m_functions.end() && m_functions.at(id) && m_functions.at(id) != nullptr;

            if (!hasParent() || isLocalOnly)
            {
                return bHasLocally;
            }

            return bHasLocally || m_parent->hasFunction(id);
            */
        }

        const Function Component::getFunction(const String& inId) const
        {
            return nullptr;

            /*
            if (inId.isEmpty())
            {
                return nullptr;
            }

            const String id = inId.split(FUNCTION_PARAMS_OPENING).front().trim();

            if (!hasParent() || isLocalOnly)
            {
                return hasFunction(id, true) ? m_functions.at(id) : nullptr;
            }

            return hasFunction(id, true) ? m_functions.at(id) : m_parent->getFunction(id);
            */
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

        float Component::getDepth() const
        {
            if (isRoot())
            {
                return 0.0f;
            }

            return std::max(m_parent->getDepth() + 0.1f, m_style.zIndex.get());
        }

        const Vec2& Component::getSize() const
        {
            return m_size;
        }

        void Component::addSize(const Vec2& inValue)
        {
            addSize(inValue.x, inValue.y);
        }

        void Component::addSize(float inWidth, float inHeight)
        {
            setSize(m_size.x + inWidth, m_size.y + inHeight);
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
            m_primitive;
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

            for (const StyleRuleset& source : m_styleFile->getRulesets())
            {
                if (source.isEmpty())
                {
                    continue;
                }

                for (const String& selector : source.selectors)
                {
                    if (hasSelector(selector.trim()))
                    {
                        addStyleProperties(source.properties);
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

            const Vec2 startMargin(m_style.margin.left.get(), m_style.margin.top.get());
            const Vec2 startPadding(m_style.padding.left.get(), m_style.padding.top.get());

            if (isRoot() || m_style.isPosition(StylePosition::Absolute))
            {
                setPosition(startMargin);
                addCursor(startPadding);

                return;
            }

            const Style& parentStyle = m_parent->getStyle();

            setPosition(m_parent->getCursor() + startMargin);
            addCursor(startPadding);

            const Vec2 endMargin(m_style.margin.right.get(), m_style.margin.bottom.get());

            switch (parentStyle.display.get())
            {
            case StyleDisplay::Flex:
                if (parentStyle.flex.direction.get() == StyleFlexDirection::Row)
                {
                    m_parent->addCursor(m_size.x + endMargin.x + m_style.gap.left.get(), 0.0f);
                }
                else
                {
                    m_parent->addCursor(0.0f, m_size.y + endMargin.y + m_style.gap.top.get());
                }

                break;

            default:
                m_parent->addCursor(0.0f, m_size.y + endMargin.y);

                break;
            }
        }

        void Component::refreshBounds()
        {
            m_bounds.top    = m_position.y;
            m_bounds.bottom = m_position.y + m_size.y;
            m_bounds.left   = m_position.x;
            m_bounds.right  = m_position.x + m_size.x;
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

        bool Component::isReference(const String& inValue) const
        {
            const bool bHasOpening = inValue.firstOf(REFERENCE_VALUE_OPENING) != String::npos;
            const bool bHasClosing = inValue.lastOf(REFERENCE_VALUE_CLOSING) != String::npos;

            return bHasOpening && bHasClosing;
        }

        String Component::parseReference(const String& inValue) const
        {
            if (!hasReference(inValue))
            {
                return inValue;
            }

            if (!hasParent())
            {
                if (const ReflectionTypeInfo* type = ReflectionTypeRegistry::get().find(typeid(*this)))
                {
                    try
                    {
                        return type->resolve(inValue).toString(this);
                    }
                    catch (const std::exception& e)
                    {
                        return inValue;
                    }
                }
            }

            return m_parent->parseReference(inValue);
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

            FunctionData data;
            data.name = trimmedValue.substr(0, inRefValue.firstOf(FUNCTION_PARAMS_OPENING) + 1);

            const String params = inRefValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING);
            for (const String& value : params.split(FUNCTION_PARAMS_SEPARATOR))
            {
                data.params.push_back(parseReference(value.trim()));
            }

            return data;
        }
    }
}