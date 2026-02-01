#include "Chicane/Grid/Component.hpp"

#include <algorithm>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            m_attributes = Xml::getAttributes(inNode);

            addChildren(inNode);
        }

        Component::Component(const String& inTag)
            : Changeable(),
              m_tag(inTag),
              m_style({}),
              m_functions({}),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_size({}),
              m_position({}),
              m_bounds({}),
              m_cursor({}),
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
            return isVisible() && hasPrimitive();
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

        bool Component::isVisible() const
        {
            const bool isParentVisible = isRoot() ? true : m_parent->isVisible();
            const bool isDisplayable =
                !m_style.isDisplay(StyleDisplay::None) && !m_style.isDisplay(StyleDisplay::Hidden);

            return isParentVisible && isDisplayable;
        }

        bool Component::isValidChild(Component* inComponent) const
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

            String onClickAttribute = getAttribute(ON_HOVER_ATTRIBUTE_NAME);

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
            return getAttribute(ID_ATTRIBUTE_NAME);
        }

        std::vector<String> Component::getClasses() const
        {
            return getClass().split(Style::CLASS_SEPARATOR);
        }

        String Component::getClass() const
        {
            return getAttribute(CLASS_ATTRIBUTE_NAME);
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

        void Component::setStyle(const StyleSource::List& inSources)
        {
            if (inSources.empty())
            {
                return;
            }

            for (const StyleSource& source : inSources)
            {
                if (source.isEmpty())
                {
                    continue;
                }

                for (const String& selector : source.selectors)
                {
                    if (selector.equals(Style::INCLUSIVE_SELECTOR))
                    {
                        setStyle(source.properties);

                        continue;
                    }

                    if (!getTag().isEmpty())
                    {
                        if (selector.equals(getTag()))
                        {
                            setStyle(source.properties);

                            continue;
                        }
                    }

                    if (!getClass().isEmpty())
                    {
                        for (const String& className : getClasses())
                        {
                            String formattedClass = "";
                            formattedClass.append(Style::CLASS_SELECTOR);
                            formattedClass.append(className);

                            if (formattedClass.equals(selector))
                            {
                                setStyle(source.properties);

                                continue;
                            }
                        }
                    }

                    if (!getId().isEmpty())
                    {
                        String formattedId = "";
                        formattedId.append(Style::ID_SELECTOR);
                        formattedId.append(getId());

                        if (formattedId.equals(selector))
                        {
                            setStyle(source.properties);

                            continue;
                        }
                    }
                }
            }

            for (Component* child : getChildren())
            {
                child->setStyle(inSources);
            }
        }

        void Component::setStyle(const StyleSource::Map& inSource)
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

            emmitChanges();
        }

        void Component::removeReference(const String& inId)
        {
            if (!hasReference(inId, true))
            {
                return;
            }

            m_references.erase(inId);

            emmitChanges();
        }

        bool Component::hasFunction(const String& inId, bool isLocalOnly) const
        {
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

            emmitChanges();
        }

        void Component::removeFunction(const String& inId)
        {
            if (!hasFunction(inId, true))
            {
                return;
            }

            m_functions.erase(inId);

            emmitChanges();
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

            setValue(m_root, inComponent);

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
            if (!isValidChild(inComponent))
            {
                return;
            }

            inComponent->setRoot(m_root);
            inComponent->setParent(this);

            m_children.push_back(inComponent);

            onAdopted(inComponent);
        }

        Vec2 Component::getAvailableSize() const
        {
            return m_size - m_cursor;
        }

        const Vec2& Component::getSize() const
        {
            return m_size;
        }

        void Component::setSize(const Vec2& inSize)
        {
            setSize(inSize.x, inSize.y);
        }

        void Component::setSize(float inWidth, float inHeight)
        {
            setValue(m_size, {inWidth, inHeight});
        }

        const Vec2& Component::getPosition() const
        {
            return m_position;
        }

        void Component::addPosition(const Vec2& inPosition)
        {
            addPosition(inPosition.x, inPosition.y);
        }

        void Component::addPosition(float inX, float inY)
        {
            setPosition(m_position.x + inX, m_position.y + inY);
        }

        void Component::setPosition(const Vec2& inPosition)
        {
            setPosition(inPosition.x, inPosition.y);
        }

        void Component::setPosition(float inX, float inY)
        {
            setValue(m_position, {inX, inY});

            setCursor(m_position);
        }

        const Vec2& Component::getCursor() const
        {
            return m_cursor;
        }

        void Component::addCursor(const Vec2& inCursor)
        {
            addCursor(inCursor.x, inCursor.y);
        }

        void Component::addCursor(float inX, float inY)
        {
            setCursor(m_cursor.x + inX, m_cursor.y + inY);
        }

        void Component::setCursor(const Vec2& inCursor)
        {
            setCursor(inCursor.x, inCursor.y);
        }

        void Component::setCursor(float inX, float inY)
        {
            setValue(m_cursor, {inX, inY});
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
            setValue(m_primitive, {});
        }

        void Component::setPrimitive(const Primitive& inPrimitive)
        {
            setValue(m_primitive, inPrimitive);
        }

        void Component::refreshStyle()
        {
            m_style.refresh();
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            setSize(m_style.width.get(), m_style.height.get());
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