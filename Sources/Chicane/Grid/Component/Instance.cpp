#include "Grid/Component/Instance.hpp"

#include "Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::unordered_map<std::string, Component::Compiler> m_components = {
            { Button::TAG_ID,      [](const pugi::xml_node& inNode) { return new Button(inNode); } },
            { List::TAG_ID,        [](const pugi::xml_node& inNode) { return new List(inNode); }  },
            { Container::TAG_ID,   [](const pugi::xml_node& inNode) { return new Container(inNode); }  },
            { Popup::TAG_ID,       [](const pugi::xml_node& inNode) { return new Popup(inNode); }  },
            { ProgressBar::TAG_ID, [](const pugi::xml_node& inNode) { return new ProgressBar(inNode); }  },
            { Text::TAG_ID,        [](const pugi::xml_node& inNode) { return new Text(inNode); }  },
            { TextInput::TAG_ID,   [](const pugi::xml_node& inNode) { return new TextInput(inNode); }  }
        };

        Component* Component::createComponent(const pugi::xml_node& inNode)
        {
            const std::string& tag = inNode.name();

            if (m_components.find(tag) == m_components.end())
            {
                return nullptr;
            }

            return m_components.at(tag)(inNode);
        }

        void Component::registerComponent(const std::string& inTag, Compiler inCompiler)
        {
            if (m_components.find(inTag) != m_components.end())
            {
                return;
            }

            m_components.insert(
                std::make_pair(
                    inTag,
                    inCompiler
                )
            );
        }

        void Component::getComponents(std::vector<const Component*>& outValue, const Component* inComponent)
        {
            outValue.push_back(inComponent);

            for (const Component* component : inComponent->getChildren())
            {
                outValue.push_back(component);

                getComponents(outValue, component);
            }
        }

        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name(), XML::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string())
        {
            setClass(XML::getAttribute(CLASS_ATTRIBUTE_NAME, inNode).as_string());
        }

        Component::Component(const std::string& inTag, const std::string& inId)
            : m_tag(inTag),
            m_id(inId),
            m_class(""),
            m_style({}),
            m_functions({}),
            m_root(nullptr),
            m_parent(nullptr),
            m_children({}),
            m_cursor({}),
            m_size({}),
            m_sizeObservable(std::make_unique<Observable<const Vec<2, float>&>>()),
            m_position({}),
            m_positionObservable(std::make_unique<Observable<const Vec<2, float>&>>())
        {}

        Component::~Component()
        {
            for (Component* child : m_children)
            {
                delete child;
            }

            m_children.clear();
        }

        bool Component::isValidChild(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
        }

        void Component::refresh()
        {
            refreshSize();
            refreshPosition();
        }

        void Component::refreshSize()
        {
            setSize(
                getSize(m_style.width, Style::Direction::Horizontal),
                getSize(m_style.height, Style::Direction::Vertical)
            );
        }

        void Component::refreshPosition()
        {
            setCursor(m_style.position == Style::Position::Absolute ? Vec2Zero : m_parent->getCursor());

            Vec<2, float> margin = Vec2Zero;

            margin.x += getSize(m_style.margin.left, Style::Direction::Horizontal);
            if (!String::areEquals(m_style.margin.left, m_style.margin.right))
            {
                margin.x -= getSize(m_style.margin.right, Style::Direction::Horizontal);
            }

            margin.y += getSize(m_style.margin.top, Style::Direction::Vertical);
            if (!String::areEquals(m_style.margin.top, m_style.margin.bottom))
            {
                margin.y -= getSize(m_style.margin.bottom, Style::Direction::Vertical);
            }

            Vec<2, float> position = m_cursor + margin;
            position.x = std::min(
                std::max(0.0f, margin.x),
                m_parent->getSize().x
            );
            position.y = std::min(
                std::max(0.0f, margin.y),
                m_parent->getSize().y
            );

            setPosition(position);

            if (m_style.position == Style::Position::Absolute)
            {
                return;
            }

            m_parent->addCursor(margin + m_size);
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

        void Component::setStyle(const Style::Sources& inSources)
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

                for (const std::string& selector : String::split(source.selector, Style::SELECTOR_SEPARATOR))
                {
                    if (String::areEquals(selector, std::to_string(Style::INCLUSIVE_SELECTOR)))
                    {
                        setStyle(source.properties);

                        continue;
                    }

                    if (!getTag().empty())
                    {
                        if (String::areEquals(selector, getTag()))
                        {
                            setStyle(source.properties);

                            continue;
                        }
                    }

                    if (!getClass().empty())
                    {
                        for (const std::string& className : String::split(getClass(), Style::CLASS_SEPARATOR))
                        {
                            if (String::areEquals(selector, Style::CLASS_SELECTOR + className))
                            {
                                setStyle(source.properties);

                                continue;
                            }
                        }
                    }

                    if (!getId().empty())
                    {
                        if (String::areEquals(selector, Style::ID_SELECTOR + getId()))
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
            m_style.update(inSource);
        }

        void Component::setStyle(const Style::Instance& inStyle)
        {
            m_style = inStyle;
        }

        bool Component::hasReference(const std::string& inId) const
        {
            return m_references.find(inId) != m_references.end() &&
                   m_references.at(inId) && !m_references.at(inId)->isEmpty();
        }

        Reference* Component::getReference(const std::string& inId) const
        {
            if (!hasReference(inId))
            {
                return nullptr;
            }

            return m_references.at(inId);
        }

        void Component::addReference(const References& inReferences)
        {
            for (auto [id, reference] : inReferences)
            {
                addReference(id, reference);
            }
        }

        void Component::addReference(const std::string& inId, Reference* inReference)
        {
            if (hasReference(inId))
            {
                return;
            }

            m_references.insert(
                std::make_pair(
                    inId,
                    inReference
                )
            );
        }

        void Component::removeReference(const std::string& inId)
        {
            if (!hasReference(inId))
            {
                return;
            }

            m_references.erase(inId);
        }

        bool Component::hasFunction(const std::string& inId) const
        {
            return m_functions.find(inId) != m_functions.end();
        }

        const Function Component::getFunction(const std::string& inId) const
        {
            if (!hasFunction(inId))
            {
                return nullptr;
            }

            return m_functions.at(inId);
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
            if (hasFunction(inId))
            {
                return;
            }

            m_functions.insert(
                std::make_pair(
                    inId,
                    inFunction
                )
            );
        }

        void Component::removeFunction(const std::string& inId)
        {
            if (!hasFunction(inId))
            {
                return;
            }

            m_functions.erase(inId);
        }

        bool Component::hasRoot() const
        {
            return m_root != nullptr && m_root != this;
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
        }

        bool Component::hasParent() const
        {
            return m_parent != nullptr && m_parent != this;
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

            if (m_parent == nullptr)
            {
                return;
            }

            m_parent->watchSize(
                [this](const Vec<2, float>& inSize)
                {
                    refreshSize();
                    refreshPosition();
                }
            );
            m_parent->watchPosition(
                [this](const Vec<2, float>& inPosition)
                {
                    refreshPosition();
                }
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

        void Component::addChildren(const pugi::xml_node& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            for (const auto& child : inNode.children())
            {
                Component* component = Component::createComponent(child);

                if (component == nullptr)
                {
                    continue;
                }

                component->setRoot(m_root);
                component->setParent(this);
                component->addChildren(child);

                addChild(component);
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

            onChildAddition(inComponent);
        }

        const Vec<2, float>& Component::getCursor() const
        {
            return m_cursor;
        }

        void Component::addCursor(const Vec<2, float>& inCursor)
        {
            setCursor(m_cursor + inCursor);
        }

        void Component::setCursor(const Vec<2, float>& inCursor)
        {
            m_cursor = inCursor;
        }

        Vec<2, float> Component::getNormalizedSize() const
        {
            return m_size / m_root->getSize();
        }

        const Vec<2, float>& Component::getSize() const
        {
            return m_size;
        }

        void Component::setSize(const Vec<2, float>& inSize)
        {
            setSize(inSize.x, inSize.y);
        }

        void Component::setSize(int inWidth, int inHeight)
        {
            if (m_size.x == inWidth && m_size.y == inWidth)
            {
                return;
            }

            m_size.x = inWidth;
            m_size.y = inHeight;

            m_sizeObservable->next(m_size);
        }

        Subscription<const Vec<2, float>&>* Component::watchSize(
            std::function<void (const Vec<2, float>&)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        ) const
        {
            Subscription<const Vec<2, float>&>* subscription = m_sizeObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_size);

            return subscription;
        }

        Vec<2, float> Component::getNormalizedPosition() const
        {
            const Vec<2, float>& size = m_root->getSize();

            Vec<2, float> result = Vec2Zero;
            result.x = (2.0f * m_position.x) / size.x - 1.0f;
            result.y = 1.0f - (2.0f * m_position.y) / size.y;

            return result;
        }

        const Vec<2, float>& Component::getPosition() const
        {
            return m_position;
        }

        void Component::setPosition(const Vec<2, float>& inPosition)
        {
            setPosition(inPosition.x, inPosition.y);
        }

        void Component::setPosition(int inX, int inY)
        {
            if (m_position.x == inX && m_position.y == inX)
            {
                return;
            }

            m_position.x = inX;
            m_position.y = inY;

            m_positionObservable->next(m_position);
        }

        Subscription<const Vec<2, float>&>* Component::watchPosition(
            std::function<void (const Vec<2, float>&)> inNext,
            std::function<void (const std::string&)> inError,
            std::function<void ()> inComplete
        ) const
        {
            Subscription<const Vec<2, float>&>* subscription = m_positionObservable->subscribe(
                inNext,
                inError,
                inComplete
            );
            subscription->next(m_position);

            return subscription;
        }

        float Component::getSizeFromPixel(const std::string& inValue)
        {
            if (!String::endsWith(inValue, PIXEL_SIZE_UNIT))
            {
                return 0.0f;
            }
        
            return std::stof(std::string(inValue.begin(), inValue.end() - 2));
        }

        float Component::getSizeFromPixel(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromPixel(inAttribute.as_string());
        }

        float Component::getSizeFromPercentage(const std::string& inValue, Style::Direction inDirection)
        {
            if (!String::endsWith(inValue, PERCENTAGE_SIZE_UNIT))
            {
                return 0.0f;
            }

            return getSizeFromPercentage(
                std::stof(std::string(inValue.begin(), inValue.end() - 1)),
                inDirection
            );
        }

        float Component::getSizeFromPercentage(float inValue, Style::Direction inDirection)
        {
            float value = inValue / 100;

            if (!hasRoot() || !hasParent())
            {
                return value;
            }

            const Vec<2, float>& size = m_style.position == Style::Position::Absolute ?
                m_root->getSize() :
                m_parent->getSize();

            if (inDirection == Style::Direction::Horizontal)
            {
                return size.x * value;
            }

            return size.y * value;
        }

        float Component::getSizeFromViewportHeight(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromViewportHeight(inAttribute.as_string());
        }

        float Component::getSizeFromViewportHeight(const std::string& inValue)
        {
            if (!String::endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return 0.0f;
            }

            return getSizeFromViewportHeight(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float Component::getSizeFromViewportHeight(float inVhValue)
        {
            return m_root->getSize().y * (inVhValue / 100);
        }

        float Component::getSizeFromViewportWidth(const pugi::xml_attribute& inAttribute)
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return getSizeFromViewportWidth(inAttribute.as_string());
        }

        float Component::getSizeFromViewportWidth(const std::string& inValue)
        {
            if (!String::endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return 0.0f;
            }

            return getSizeFromViewportWidth(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float Component::getSizeFromViewportWidth(float inVwValue)
        {
            return  m_root->getSize().x * (inVwValue / 100);
        }

        float Component::getSize(const std::string& inValue, Style::Direction inDirection)
        {
            if (inValue.empty())
            {
                return 0.0f;
            }

            if (String::areEquals(inValue, AUTO_SIZE_UNIT))
            {
                return getSizeFromPercentage("100%", inDirection);
            }

            if (String::endsWith(inValue, PIXEL_SIZE_UNIT))
            {
                return getSizeFromPixel(inValue);
            }

            if (String::endsWith(inValue, PERCENTAGE_SIZE_UNIT))
            {
                return getSizeFromPercentage(inValue, inDirection);
            }

            if (String::endsWith(inValue, VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return getSizeFromViewportHeight(inValue);
            }

            if (String::endsWith(inValue, VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return getSizeFromViewportWidth(inValue);
            }

            std::string parsedValue = parseReference(inValue).toString();

            if (parsedValue.empty())
            {
                return 0.0f;
            }

            return std::stof(parsedValue);
        }

        FunctionData Component::parseFunction(const std::string& inRefValue)
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

            for (std::string& value : String::split(params, ','))
            {
                data.params.push_back(
                    parseReference(String::trim(value))
                );
            }

            return data;
        }

        bool Component::doesTextContainsReference(const std::string& inText)
        {
            bool hasOpening = inText.find_first_of(REF_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inText.find_first_of(REF_VALUE_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        bool Component::doesTextContainsFunction(const std::string& inValue)
        {
            bool hasOpening = inValue.find_first_of(FUNCTION_PARAMS_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_first_of(FUNCTION_PARAMS_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        Reference Component::parseReference(const std::string& inValue)
        {
            if (!hasRoot())
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            if (!doesTextContainsFunction(inValue))
            {
                if (!m_root->hasReference(inValue))
                {
                    return Reference::fromValue<const std::string>(&inValue);
                }

                return *m_root->getReference(inValue);
            }

            FunctionData functionData = parseFunction(inValue);
            Function functionRef      = m_root->getFunction(functionData.name);

            if (!functionRef)
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            Event event = {};
            event.values = functionData.params;

            return functionRef(event);
        }

        std::string Component::parseText(const std::string& inText)
        {
            if (String::trim(inText).empty())
            {
                return "";
            }

            if (!doesTextContainsReference(inText))
            {
                return inText;
            }

            std::size_t foundOpening = inText.find_first_of(REF_VALUE_OPENING);
            std::size_t foundClosing = inText.find_first_of(REF_VALUE_CLOSING);
            std::string resultText = inText.substr(0, foundOpening);
            std::string remainderText = inText.substr(
                foundClosing + 2,
                inText.size() - foundClosing
            );

            foundOpening += 2;
            foundClosing -= foundOpening;

            std::string value = inText.substr(foundOpening, foundClosing);
            value = String::trim(value);

            if (!value.empty())
            {
                resultText += parseReference(value).toString();
            }

            if (!remainderText.empty())
            {
                resultText += parseText(remainderText);
            }

            return resultText;
        }
    }
}