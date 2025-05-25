#include "Chicane/Grid/Component/Instance.hpp"

#include "Chicane/Grid/Loader.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            setId(XML::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string());
            setClass(XML::getAttribute(CLASS_ATTRIBUTE_NAME, inNode).as_string());

            addChildren(inNode);
        }

        Component::Component(const std::string& inTag)
            : m_tag(inTag),
            m_id(""),
            m_class(""),
            m_style(Style::Instance()),
            m_functions({}),
            m_root(nullptr),
            m_parent(nullptr),
            m_children({}),
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
            }

            m_children.clear();
        }

        bool Component::isDrawable() const
        {
            return isVisible() && hasPrimitive();
        }

        void Component::handle(const SDL_Event& inEvent)
        {
            onEvent(inEvent);
        }

        void Component::tick(float inDelta)
        {
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
            const bool isDisplayable   = !m_style.isDisplay(Style::Display::None) &&
                                         !m_style.isDisplay(Style::Display::Hidden);

            return isParentVisible && isDisplayable;
        }

        bool Component::isValidChild(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
        }

        const std::string& Component::getTag() const
        {
            return m_tag;
        }

        void Component::setTag(const std::string& inTag)
        {
            m_tag = inTag;
        }

        const std::string& Component::getId() const
        {
            return m_id;
        }

        void Component::setId(const std::string& inId)
        {
            m_id = inId;
        }

        const std::vector<std::string> Component::getClasses() const
        {
            return String::split(m_class, Style::CLASS_SEPARATOR);
        }

        const std::string& Component::getClass() const
        {
            return m_class;
        }

        void Component::setClass(const std::string& inClass)
        {
            m_class = inClass;
        }
        
        const Style::Instance& Component::getStyle() const
        {
            return m_style;
        }

        void Component::setStyle(const Style::Source::List& inSources)
        {
            if (inSources.empty())
            {
                return;
            }

            for (const Style::Source& source : inSources)
            {
                if (source.isEmpty())
                {
                    continue;
                }

                for (const std::string& selector : source.selectors)
                {
                    if (String::areEquals(selector, Style::INCLUSIVE_SELECTOR))
                    {
                        setStyle(source.properties);

                        continue;
                    }

                    if (!m_tag.empty())
                    {
                        if (String::areEquals(selector, m_tag))
                        {
                            setStyle(source.properties);

                            continue;
                        }
                    }

                    if (!m_class.empty())
                    {
                        for (const std::string& className : getClasses())
                        {
                            std::string formattedClass = "";
                            formattedClass.push_back(Style::CLASS_SELECTOR);
                            formattedClass.append(className);

                            if (String::areEquals(selector, formattedClass))
                            {
                                setStyle(source.properties);

                                continue;
                            }
                        }
                    }

                    if (!m_id.empty())
                    {
                        std::string formattedId = "";
                        formattedId.push_back(Style::ID_SELECTOR);
                        formattedId.append(m_id);

                        if (String::areEquals(selector, formattedId))
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

        void Component::setStyle(const Style::Properties& inSource)
        {
            m_style.setProperties(inSource);
        }

        bool Component::hasReference(const std::string& inId, bool isLocalOnly) const
        {
            bool wasFoundLocally = m_references.find(inId) != m_references.end() &&
                                   m_references.at(inId) && !m_references.at(inId)->isEmpty();

            if (isRoot() || !hasParent() || isLocalOnly)
            {
                return wasFoundLocally;
            }

            return wasFoundLocally || m_parent->hasReference(inId);
        }

        Reference* Component::getReference(const std::string& inId) const
        {
            if (!hasParent() || isRoot())
            {
                return m_references.at(inId);
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

        void Component::addReference(const std::string& inId, Reference* inReference)
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

        void Component::removeReference(const std::string& inId)
        {
            if (!hasReference(inId, true))
            {
                return;
            }

            m_references.erase(inId);

            emmitChanges();
        }

        bool Component::hasFunction(const std::string& inId, bool isLocalOnly) const
        {
            const std::string id = String::split(inId, FUNCTION_PARAMS_OPENING).front();

            bool hasLocally = m_functions.find(id) != m_functions.end() &&
                              m_functions.at(id) && m_functions.at(id) != nullptr;

            if (!hasParent() || isRoot() || isLocalOnly)
            {
                return hasLocally;
            }

            return hasLocally || m_parent->hasFunction(id);
        }

        const Function Component::getFunction(const std::string& inId) const
        {
            const std::string id = String::split(inId, FUNCTION_PARAMS_OPENING).front();

            if (!hasParent() || isRoot())
            {
                return m_functions.at(id);
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

        void Component::addFunction(const std::string& inId, Function inFunction)
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

        void Component::removeFunction(const std::string& inId)
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
                addChild(Loader::createComponent(child));
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

            emmitChanges();
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

            if (isRoot() || m_style.isPosition(Style::Position::Absolute))
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

        std::string Component::parseText(const std::string& inValue) const
        {            
            if (inValue.empty())
            {
                return "";
            }

            if (!isReference(inValue))
            {
                return inValue;
            }

            const std::uint32_t start = inValue.find_first_of(REFERENCE_VALUE_OPENING) + 1;
            const std::uint32_t end   = inValue.find_last_of(REFERENCE_VALUE_CLOSING) - 1;

            const std::string value     = String::trim(inValue.substr(start + 1, end - start - 1));
            const std::string remainder = inValue.substr(end + 2);

            std::string result = "";

            if (!value.empty())
            {
                result += parseReference(value).toString();
            }

            if (!remainder.empty())
            {
                result += parseText(remainder);
            }

            return result;
        }

        bool Component::isReference(const std::string& inValue) const
        {
            bool hasOpening = inValue.find_first_of(REFERENCE_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_last_of(REFERENCE_VALUE_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        Reference Component::parseReference(const std::string& inValue) const
        {
            if (!isFunction(inValue))
            {
                if (hasReference(inValue))
                {
                    return *getReference(inValue);
                }

                return Reference::fromValue<const std::string>(&inValue);
            }

            if (!hasFunction(inValue))
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            FunctionData data = parseFunction(inValue);

            Event event = {};
            event.values = data.params;

            return getFunction(data.name)(event);
        }

        bool Component::isFunction(const std::string& inValue) const
        {
            bool hasOpening = inValue.find_first_of(FUNCTION_PARAMS_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_last_of(FUNCTION_PARAMS_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        FunctionData Component::parseFunction(const std::string& inRefValue) const
        {
            std::string trimmedValue = String::trim(inRefValue);

            if (trimmedValue.empty())
            {
                return {};
            }

            std::size_t paramsStart = trimmedValue.find_first_of(FUNCTION_PARAMS_OPENING);
            std::size_t paramsEnd   = trimmedValue.find_last_of(FUNCTION_PARAMS_CLOSING);
            std::string name        = trimmedValue.substr(0, paramsStart);

            paramsStart += 1;
            paramsEnd   -= paramsStart;

            std::string params = String::trim(
                inRefValue.substr(
                    paramsStart,
                    paramsEnd
                )
            );

            FunctionData data = {};
            data.name = name;

            for (std::string& value : String::split(params, FUNCTION_PARAMS_SEPARATOR))
            {
                data.params.push_back(parseReference(String::trim(value)));
            }

            return data;
        }
    }
}