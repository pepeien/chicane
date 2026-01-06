#include "Chicane/Grid/Component.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            setId(Xml::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string());
            setClass(Xml::getAttribute(CLASS_ATTRIBUTE_NAME, inNode).as_string());

            addChildren(inNode);
        }

        Component::Component(const String& inTag)
            : m_tag(inTag),
            m_id(""),
            m_class(""),
            m_style(Style()),
            m_functions({}),
            m_root(nullptr),
            m_parent(nullptr),
            m_children({}),
            m_childrenObservable({}),
            m_size({}),
            m_position({}),
            m_cursor({}),
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
            onTick(inDeltaTime);

            refresh();

            for (Component* child : m_children)
            {
                child->tick(inDeltaTime);
            }
        }

        void Component::refresh()
        {
            refreshStyle();

            if (!isVisible())
            {
                return;
            }

            refreshSize();
            refreshPosition();
            refreshPrimitive();
            refreshZIndex();

            onRefresh();
        }

        bool Component::isRoot() const
        {
            return (!m_parent && !m_root) || (m_parent == this && m_root == this);
        }

        bool Component::isVisible() const
        {
            const bool isParentVisible = isRoot() ? true : m_parent->isVisible();
            const bool isDisplayable   = !m_style.isDisplay(StyleDisplay::None) &&
                                         !m_style.isDisplay(StyleDisplay::Hidden);

            return isParentVisible && isDisplayable;
        }

        bool Component::isValidChild(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
        }

        const String& Component::getTag() const
        {
            return m_tag;
        }

        void Component::setTag(const String& inTag)
        {
            setProperty(m_tag, inTag);
        }

        const String& Component::getId() const
        {
            return m_id;
        }

        void Component::setId(const String& inId)
        {
            setProperty(m_id, inId);
        }

        const std::vector<String> Component::getClasses() const
        {
            return m_class.split(Style::CLASS_SEPARATOR);
        }

        const String& Component::getClass() const
        {
            return m_class;
        }

        void Component::setClass(const String& inClass)
        {
            setProperty(m_class, inClass);
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

                    if (!m_tag.isEmpty())
                    {
                        if (selector.equals(m_tag))
                        {
                            setStyle(source.properties);

                            continue;
                        }
                    }

                    if (!m_class.isEmpty())
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

                    if (!m_id.isEmpty())
                    {
                        String formattedId = "";
                        formattedId.append(Style::ID_SELECTOR);
                        formattedId.append(m_id);

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
            bool bWasFoundLocally = m_references.find(inId) != m_references.end() &&
                                    m_references.at(inId) && !m_references.at(inId)->isEmpty();

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

            const bool bHasLocally = m_functions.find(id) != m_functions.end() &&
                                     m_functions.at(id) && m_functions.at(id) != nullptr;

            if (!hasParent() || isRoot() || isLocalOnly)
            {
                return bHasLocally;
            }

            return bHasLocally || m_parent->hasFunction(id);
        }

        const Function Component::getFunction(const String& inId) const
        {
            const String id = inId.split(FUNCTION_PARAMS_OPENING).front().trim();

            if (!hasParent() || isRoot())
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

            setProperty(m_root, inComponent);

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
                [&](Component* children)
                {
                    return children == this;
                }
            ) - neighbours.begin();

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

        Component::ChildrenSubscription Component::watchChildren(
            ChildrenSubscription::NextCallback inNext,
            ChildrenSubscription::ErrorCallback inError,
            ChildrenSubscription::CompleteCallback inComplete
        )
        {
            return m_childrenObservable
                .subscribe(inNext, inError, inComplete)
                .next(this);
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
            setProperty(m_cursor, { inX, inY });
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
            setProperty(m_size, { inWidth, inHeight });
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
            setProperty(m_position, { inX, inY });

            setCursor(m_position);
        }

        Vec2 Component::getCenter() const
        {
            return m_size * 0.5f;
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
            setProperty(m_primitive, {});
        }

        void Component::setPrimitive(const Primitive& inPrimitive)
        {
            setProperty(m_primitive, inPrimitive);
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

            setSize(m_style.width, m_style.height);
        }

        void Component::refreshPosition()
        {
            setPosition(0.0f, 0.0f);

            Vec2 margin = Vec2(
                m_style.margin.left == m_style.margin.right  ? m_style.margin.left : (m_style.margin.left - m_style.margin.right),
                m_style.margin.top  == m_style.margin.bottom ? m_style.margin.top  : (m_style.margin.top  - m_style.margin.bottom)
            );
            Vec2 padding = Vec2(
                m_style.padding.left == m_style.padding.right  ? m_style.padding.left : (m_style.padding.left - m_style.padding.right),
                m_style.padding.top  == m_style.padding.bottom ? m_style.padding.top  : (m_style.padding.top  - m_style.padding.bottom)
            );

            if (isRoot() || m_style.isPosition(StylePosition::Absolute))
            {
                setPosition(margin);
                addCursor(padding);

                return;
            }

            const Style& parentStyle = m_parent->getStyle();
            const Vec2& parentCursor = m_parent->getCursor();

            const Vec2 addedSpacing  = margin;
            const Vec2 occupiedSpace = m_size + margin;

            setPosition(parentCursor + addedSpacing);
            addCursor(padding);

            switch (parentStyle.display)
            {
            case StyleDisplay::Flex:
                if (parentStyle.flex.direction == StyleFlex::Direction::Row)
                {
                    m_parent->addCursor(parentStyle.gap.left + occupiedSpace.x, 0.0f);
                }

                if (parentStyle.flex.direction == StyleFlex::Direction::Column)
                {
                    m_parent->addCursor(0.0f, parentStyle.gap.top + occupiedSpace.y);
                }

                break;

            default:
                m_parent->addCursor(0.0f, occupiedSpace.y);

                break;
            }
        }

        void Component::refreshZIndex()
        {
            const Style& parentStyle = getParent()->getStyle();

            if (m_style.zIndex > 0.0f || parentStyle.zIndex <= 0.0f)
            {
                return;
            }

            m_style.zIndex = parentStyle.zIndex + 0.001f;
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

            Event event = {};
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
            data.name = trimmedValue.substr(0, inRefValue.firstOf(FUNCTION_PARAMS_OPENING) + 1);

            for (const String& value : params.split(FUNCTION_PARAMS_SEPARATOR))
            {
                data.params.push_back(parseReference(value.trim()));
            }

            return data;
        }
    }
}