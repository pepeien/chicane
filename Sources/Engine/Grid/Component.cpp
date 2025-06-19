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

        void Component::tick(float inDelta)
        {
            refresh();

            onTick(inDelta);

            for (Component* child : m_children)
            {
                child->tick(inDelta);
            }
        }

        void Component::refresh()
        {
            refreshStyle();
            refreshSize();
            refreshPosition();
            refreshPrimitive();

            onRefresh();
        }

        bool Component::isRoot() const
        {
            return (
                (!m_parent && !m_root) ||
                (m_parent == this && m_root == this)
            );
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
            m_tag = inTag;
        }

        const String& Component::getId() const
        {
            return m_id;
        }

        void Component::setId(const String& inId)
        {
            m_id = inId;
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
            m_class = inClass;
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

        void Component::setStyle(const StyleProperties& inSource)
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

            m_references.insert(
                std::make_pair(
                    inId,
                    inReference
                )
            );

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
            const String id = inId.split(FUNCTION_PARAMS_OPENING).front();

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
            const String& id = inId.split(FUNCTION_PARAMS_OPENING).front();

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

            m_functions.insert(
                std::make_pair(
                    inId,
                    inFunction
                )
            );

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
            inComponent->watchChanges(
                [this, inComponent]()
                {
                    m_childrenObservable.next(inComponent);
                }
            );
            inComponent->watchChildren(
                [this](Grid::Component* component)
                {
                    m_childrenObservable.next(component);
                }
            );

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
            Vec2 cursor = m_cursor;
            cursor.x += inX;

            if (cursor.x >= m_size.x)
            {
                cursor.x = 0;
                cursor.y = inY;
            }

            setCursor(cursor);
        }

        void Component::setCursor(const Vec2& inCursor)
        {
            setCursor(inCursor.x, inCursor.y);
        }

        void Component::setCursor(float inX, float inY)
        {
            if (
                std::fabs(m_cursor.x - inX) < FLT_EPSILON &&
                std::fabs(m_cursor.y - inY) < FLT_EPSILON
            )
            {
                return;
            }

            m_cursor.x = inX;
            m_cursor.y = inY;
        }

        Vec2 Component::getAvailableSize() const
        {
            return m_size;
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

        void Component::setPosition(const Vec2& inPosition)
        {
            setPosition(inPosition.x, inPosition.y);
        }

        void Component::setPosition(float inX, float inY)
        {
            if (
                std::fabs(m_position.x - inX) < FLT_EPSILON &&
                std::fabs(m_position.y - inY) < FLT_EPSILON
            )
            {
                return;
            }

            m_position.x = inX;
            m_position.y = inY;

            setCursor(m_position);
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
            setCursor(0.0f, 0.0f);

            Vec2 margin = Vec2(
                m_style.margin.left - m_style.margin.right,
                m_style.margin.top - m_style.margin.bottom
            );

            if (isRoot() || m_style.isPosition(StylePosition::Absolute))
            {
                setPosition(margin);

                return;
            }

            const Vec2 cursor = m_parent->getCursor();

            const Vec2 parentCenter = m_parent->getSize() * 0.5f;
  
            const Vec2 position = cursor + margin;

            Vec2 offset = { m_size.x * 0.5f, m_size.y * 0.5f };

            if (position.x >= parentCenter.x)
            {
                offset.x *= -1.0f;
            }

            if (position.y >= parentCenter.y)
            {
                offset.y *= -1.0f;
            }

            margin.x += offset.x;
            margin.y += offset.y;

            const Vec2 occupiedSpace = m_size + margin;

            m_parent->addCursor(occupiedSpace.x, 0.0f);

            setPosition(cursor + margin);
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

            const String value     = inValue.substr(start + 1, end - start - 1).trim();
            const String remainder = inValue.substr(end + 2);

            String result = "";

            if (!value.isEmpty())
            {
                result += parseReference(value).toString();
            }

            if (!remainder.isEmpty())
            {
                result += parseText(remainder);
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

            for (String& value : params.split(FUNCTION_PARAMS_SEPARATOR))
            {
                data.params.push_back(parseReference(value.trim()));
            }

            return data;
        }
    }
}