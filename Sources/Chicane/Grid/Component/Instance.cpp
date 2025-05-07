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
            if (inComponent->isDrawable())
            {
                outValue.push_back(inComponent);
            }

            for (const Component* component : inComponent->getChildren())
            {
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

        void Component::onChildAddition(Component* inComponent)
        {
            return;
        }

        void Component::onTick(float inDelta)
        {
            return;
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

        bool Component::isValidChild(Component* inComponent) const
        {
            return inComponent != nullptr && inComponent != this;
        }

        bool Component::isDrawable() const
        {
            const std::string& backgroundColor = m_style.backgroundColor;

            bool isBackgroundTransparent = backgroundColor.empty() || String::areEquals(
                backgroundColor,
                Style::BACKGROUND_COLOR_TRANSPARENT
            );

            bool isHidden = m_style.display != Style::Display::Visible;

            return !isBackgroundTransparent && !isHidden;
        }

        void Component::refresh()
        {
            refreshSize();
            refreshPosition();
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

                for (const std::string& selector : source.selectors)
                {
                    if (String::areEquals(selector, std::to_string(Style::INCLUSIVE_SELECTOR)))
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
            m_style.update(inSource);
        }

        void Component::setStyle(const Style::Instance& inStyle)
        {
            m_style = inStyle;
        }

        bool Component::hasReference(const std::string& inId, bool isLocalOnly) const
        {
            bool wasFoundLocally = m_references.find(inId) != m_references.end() &&
                                   m_references.at(inId) && !m_references.at(inId)->isEmpty();

            if (!hasParent() || isLocalOnly)
            {
                return wasFoundLocally;
            }

            return wasFoundLocally || m_parent->hasReference(inId);
        }

        Reference* Component::getReference(const std::string& inId) const
        {
            if (!hasReference(inId))
            {
                return nullptr;
            }

            if (!hasParent())
            {
                return m_references.at(inId);
            }

            return hasReference(inId, true) ? m_references.at(inId) : m_parent->getReference(inId);
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
        }

        void Component::removeReference(const std::string& inId)
        {
            if (!hasReference(inId, true))
            {
                return;
            }

            m_references.erase(inId);
        }

        bool Component::hasFunction(const std::string& inId, bool isLocalOnly) const
        {
            bool isLocal = m_functions.find(inId) != m_functions.end() &&
                           m_functions.at(inId) && m_functions.at(inId) != nullptr;

            if (!hasParent() || isLocalOnly)
            {
                return isLocal;
            }

            return isLocal || m_parent->hasFunction(inId);
        }

        const Function Component::getFunction(const std::string& inId) const
        {
            if (!hasFunction(inId))
            {
                return nullptr;
            }

            if (!hasParent())
            {
                return m_functions.at(inId);
            }

            return hasFunction(inId, true) ? m_functions.at(inId) : m_parent->getFunction(inId);

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
        }

        void Component::removeFunction(const std::string& inId)
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
            m_position.x = inX;
            m_position.y = inY;

            setCursor(m_position);

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

        Vec<4, std::uint32_t> Component::getBackgroundColor() const
        {
            const std::string color = m_style.backgroundColor;

            if (
                String::startsWith(color, Style::RGB_KEYWORD) ||
                String::startsWith(color, Style::RGBA_KEYWORD)
            )
            {
                const std::string keyword = String::startsWith(color, Style::RGBA_KEYWORD) ?
                    Style::RGBA_KEYWORD :
                    Style::RGB_KEYWORD;

                std::string result = "";
                result.append(keyword);
                result.append(FUNCTION_PARAMS_OPENING);

                for (std::string value : String::split(extractParams(color), FUNCTION_PARAMS_SEPARATOR[0]))
                {
                    result.append(parseText(Style::colorToReference(value)));
                    result.append(",");
                }

                result.append(FUNCTION_PARAMS_CLOSING);

                return Style::toRgba(result);
            }

            return Style::toRgba(
                parseText(
                    Style::colorToReference(color)
                )
            );
        }

        void Component::refreshSize()
        {
            setSize(
                calculateSize(m_style.width, Style::Direction::Horizontal),
                calculateSize(m_style.height, Style::Direction::Vertical)
            );
        }

        void Component::refreshPosition()
        {
            setCursor(
                m_style.position == Style::Position::Absolute ? Vec2Zero : m_parent->getCursor()
            );

            Vec<2, float> margin = Vec2Zero;

            margin.x = calculateSize(m_style.margin.left, Style::Direction::Horizontal);
            if (!String::areEquals(m_style.margin.left, m_style.margin.right))
            {
                float rightMargin = calculateSize(m_style.margin.right, Style::Direction::Horizontal);

                if (margin.x > 0.0f && rightMargin > 0.0f)
                {
                    rightMargin *= -1.0f;
                }

                margin.x += rightMargin;
            }
            else
            {
                margin.x *= 0.5f;
            }

            margin.y = calculateSize(m_style.margin.top, Style::Direction::Vertical);
            if (!String::areEquals(m_style.margin.top, m_style.margin.bottom))
            {
                float bottomMargin = calculateSize(m_style.margin.bottom, Style::Direction::Vertical);

                if (margin.y > 0.0f && bottomMargin > 0.0f)
                {
                    bottomMargin *= -1.0f;
                }

                margin.y += bottomMargin;
            }
            else
            {
                margin.y *= 0.5f;
            }

            setPosition(m_cursor + margin);

            if (m_style.position == Style::Position::Absolute)
            {
                return;
            }

            m_parent->addCursor(m_size + margin);
        }

        void Component::refreshBackgroundColor()
        {
            
        }

        float Component::calculateSizeFromPixel(const pugi::xml_attribute& inAttribute) const
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return calculateSizeFromPixel(inAttribute.as_string());
        }

        float Component::calculateSizeFromPixel(const std::string& inValue) const
        {
            if (!String::endsWith(inValue, Style::PIXEL_SIZE_UNIT))
            {
                return 0.0f;
            }
        
            return std::stof(std::string(inValue.begin(), inValue.end() - 2));
        }

        float Component::calculateSizeFromPercentage(const std::string& inValue, Style::Direction inDirection) const
        {
            if (!String::endsWith(inValue, Style::PERCENTAGE_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromPercentage(
                std::stof(std::string(inValue.begin(), inValue.end() - 1)),
                inDirection
            );
        }

        float Component::calculateSizeFromPercentage(float inValue, Style::Direction inDirection) const
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

        float Component::calculateSizeFromViewportHeight(const pugi::xml_attribute& inAttribute) const
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return calculateSizeFromViewportHeight(inAttribute.as_string());
        }

        float Component::calculateSizeFromViewportHeight(const std::string& inValue) const
        {
            if (!String::endsWith(inValue, Style::VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportHeight(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float Component::calculateSizeFromViewportHeight(float inVhValue) const
        {
            return m_root->getSize().y * (inVhValue / 100);
        }

        float Component::calculateSizeFromViewportWidth(const pugi::xml_attribute& inAttribute) const
        {
            if (inAttribute.empty())
            {
                return 0.0f;
            }

            return calculateSizeFromViewportWidth(inAttribute.as_string());
        }

        float Component::calculateSizeFromViewportWidth(const std::string& inValue) const
        {
            if (!String::endsWith(inValue, Style::VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromViewportWidth(
                std::stof(std::string(inValue.begin(), inValue.end() - 2))
            );
        }

        float Component::calculateSizeFromViewportWidth(float inVwValue) const
        {
            return  m_root->getSize().x * (inVwValue / 100);
        }

        float Component::calculateSize(const std::string& inValue, Style::Direction inDirection) const
        {
            if (inValue.empty())
            {
                return 0.0f;
            }

            if (String::startsWith(inValue, Style::VARIABLE_KEYWORD))
            {
                const std::string parsedReference = parseText(Style::variableToReference(inValue));

                if (parsedReference.empty())
                {
                    return 0.0f;
                }

                return calculateSize(parsedReference, inDirection);
            }

            if (String::startsWith(inValue, Style::CALCULATION_KEYWORD))
            {
                const std::string operation = extractParams(inValue);

                for (std::uint32_t i = 0; i < operation.size(); i++)
                {
                    const std::string operatorSymbol = std::string(1, operation.at(i));

                    const auto& iterator = std::find(
                        Style::CALCULATION_OPERATORS.begin(),
                        Style::CALCULATION_OPERATORS.end(),
                        operatorSymbol
                    );

                    if (iterator == Style::CALCULATION_OPERATORS.end())
                    {
                        continue;
                    }

                    const std::string left  = operation.substr(0, i);
                    const std::string right = operation.substr(i + 1);

                    if (String::areEquals(operatorSymbol, Style::CALCULATION_OPERATOR_SUM))
                    {
                        return calculateSize(left, inDirection) + calculateSize(right, inDirection);
                    }

                    if (String::areEquals(operatorSymbol, Style::CALCULATION_OPERATOR_SUB))
                    {
                        return calculateSize(left, inDirection) - calculateSize(right, inDirection);
                    }

                    if (String::areEquals(operatorSymbol, Style::CALCULATION_OPERATOR_MUL))
                    {
                        return calculateSize(left, inDirection) * calculateSize(right, inDirection);
                    }

                    if (String::areEquals(operatorSymbol, Style::CALCULATION_OPERATOR_DIV))
                    {
                        return calculateSize(left, inDirection) / calculateSize(right, inDirection);
                    }

                    return 0.0f;
                }

                return 0.0f;
            }

            if (String::areEquals(inValue, Style::AUTO_SIZE_UNIT))
            {
                return calculateSizeFromPercentage("100%", inDirection);
            }

            if (String::endsWith(inValue, Style::PERCENTAGE_SIZE_UNIT))
            {
                return calculateSizeFromPercentage(inValue, inDirection);
            }

            if (String::endsWith(inValue, Style::VIEWPORT_HEIGHT_SIZE_UNIT))
            {
                return calculateSizeFromViewportHeight(inValue);
            }

            if (String::endsWith(inValue, Style::VIEWPORT_WIDTH_SIZE_UNIT))
            {
                return calculateSizeFromViewportWidth(inValue);
            }

            if (String::endsWith(inValue, Style::PIXEL_SIZE_UNIT))
            {
                return calculateSizeFromPixel(inValue);
            }

            return 0.0f;
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

            for (std::string& value : String::split(params, ','))
            {
                data.params.push_back(
                    parseReference(String::trim(value))
                );
            }

            return data;
        }

        bool Component::doesTextContainsReference(const std::string& inValue) const
        {
            bool hasOpening = inValue.find_first_of(REFERENCE_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_last_of(REFERENCE_VALUE_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        bool Component::doesTextContainsFunction(const std::string& inValue) const
        {
            bool hasOpening = inValue.find_first_of(FUNCTION_PARAMS_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_last_of(FUNCTION_PARAMS_CLOSING) != std::string::npos;

            return hasOpening && hasClosing;
        }

        Reference Component::parseReference(const std::string& inValue) const
        {
            if (!doesTextContainsFunction(inValue))
            {
                if (!hasReference(inValue))
                {
                    return Reference::fromValue<const std::string>(&inValue);
                }

                return *getReference(inValue);
            }

            if (!hasFunction(inValue))
            {
                return Reference::fromValue<const std::string>(&inValue);
            }

            FunctionData data = parseFunction(inValue);

            Event event = {};
            event.values = data.params;

            return m_parent->getFunction(data.name)(event);
        }

        std::string Component::parseText(const std::string& inValue) const
        {            
            if (String::trim(inValue).empty())
            {
                return "";
            }

            if (!doesTextContainsReference(inValue))
            {
                return inValue;
            }

            const std::uint32_t start = inValue.find_first_of(REFERENCE_VALUE_OPENING) + 1;
            const std::uint32_t end   = inValue.find_last_of(REFERENCE_VALUE_CLOSING) - 1;

            const std::string value     = inValue.substr(start + 1, end - start - 1);
            const std::string remainder = inValue.substr(end);

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
    }
}