#include "Chicane/Grid/Component/Instance.hpp"

#include "Chicane/Grid/Essential.hpp"
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
        }

        Component::Component(const std::string& inTag)
            : m_tag(inTag),
            m_id(""),
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
            refreshSize();
            refreshPosition();

            onTick(inDelta);

            for (Component* child : m_children)
            {
                child->tick(inDelta);
            }
        }

        bool Component::isRoot() const
        {
            return m_parent == this && m_root == this;
        }

        bool Component::isVisible() const
        {
            const bool isParentVisible = isRoot() ? true : m_parent->isVisible();
            const bool isDisplayable   = !isDisplayStyle(Style::Display::None) &&
                                         !isDisplayStyle(Style::Display::Hidden);

            return isParentVisible && isDisplayable;
        }

        bool Component::isDrawable() const
        {
            const bool isBackgroundVisible = Style::isColorVisible(getBackgroundColorStyle());

            return isVisible() && isBackgroundVisible;
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

            if (!hasParent() || isRoot() || isLocalOnly)
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

            if (!hasParent() || isRoot())
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

            if (!hasParent() || isRoot() || isLocalOnly)
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

            if (!hasParent() || isRoot())
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

            if (!m_parent || isRoot())
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
                Component* component = createComponent(child);

                if (!component)
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

        bool Component::isDisplayStyle(Style::Display inDisplay) const
        {
            return getDisplayStyle() == inDisplay;
        }

        Style::Display Component::getDisplayStyle() const
        {
            std::string value = m_style.display;

            if (String::startsWith(value, Style::VARIABLE_KEYWORD))
            {
                value = parseText(Style::variableToReference(value));
            }

            if (String::areEquals(value, Style::DISPLAY_TYPE_VISIBLE))
            {
                return Style::Display::Visible;
            }

            if (String::areEquals(value, Style::DISPLAY_TYPE_FLEX))
            {
                return Style::Display::Flex;
            }

            if (String::areEquals(value, Style::DISPLAY_TYPE_HIDDEN))
            {
                return Style::Display::Hidden;
            }

            return Style::Display::None;
        }

        float Component::getWidthStyle() const
        {
            return calculateSize(m_style.width, Style::Direction::Horizontal);
        }

        float Component::getHeightStyle() const
        {
            return calculateSize(m_style.height, Style::Direction::Vertical);
        }

        bool Component::isPositionStyle(Style::Position inPosition) const
        {
            return getPositionStyle() == inPosition;
        }

        Style::Position Component::getPositionStyle() const
        {
            if (String::areEquals(m_style.position, Style::POSITION_TYPE_ABSOLUTE))
            {
                return Style::Position::Absolute;
            }

            return Style::Position::Relative;
        }

        std::string Component::getBackgroundColorStyle() const
        {
            std::string value = m_style.backgroundColor;

            if (
                String::startsWith(value, Style::RGB_KEYWORD) ||
                String::startsWith(value, Style::RGBA_KEYWORD)
            )
            {
                const std::string keyword = String::startsWith(value, Style::RGBA_KEYWORD) ?
                    Style::RGBA_KEYWORD :
                    Style::RGB_KEYWORD;

                std::string result = "";
                result.append(keyword);
                result.push_back(FUNCTION_PARAMS_OPENING);

                for (std::string value : String::split(extractParams(value), FUNCTION_PARAMS_SEPARATOR))
                {
                    result.append(parseText(Style::colorToReference(value)));
                    result.append(",");
                }

                result.push_back(FUNCTION_PARAMS_CLOSING);

                value = result;
            }
            else
            {
                value = parseText(value);
            }

            return value;
        }

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            setSize(getWidthStyle(), getHeightStyle());
        }

        void Component::refreshPosition()
        {
            if (isRoot())
            {
                setCursor(Vec2Zero);

                return;
            }

            Vec<2, float> margin = Vec2Zero;

            const Vec<2, float>&       size = getSize();

            setCursor(
                isPositionStyle(Style::Position::Absolute) ?
                Vec2Zero :
                m_parent->getCursor()
            );

            float top    = calculateSize(m_style.margin.top,    Style::Direction::Vertical);
            float right  = calculateSize(m_style.margin.right,  Style::Direction::Horizontal);
            float bottom = calculateSize(m_style.margin.bottom, Style::Direction::Vertical);
            float left   = calculateSize(m_style.margin.left,   Style::Direction::Horizontal);

            if (
                String::areEquals(m_style.margin.left, Style::AUTO_SIZE_UNIT) &&
                String::areEquals(m_style.margin.right, Style::AUTO_SIZE_UNIT)
            )
            {
                margin.x = left * 0.5f;
            }
            else
            {
                margin.x  = left;
                margin.x -= right;
            }

            if (
                String::areEquals(m_style.margin.top, Style::AUTO_SIZE_UNIT) &&
                String::areEquals(m_style.margin.bottom, Style::AUTO_SIZE_UNIT)
            )
            {
                margin.y = top * 0.5f;
            }
            else
            {
                margin.y  = top;
                margin.y -= bottom;
            }

            if (isPositionStyle(Style::Position::Relative))
            {
                const Vec<2, float> parentCenter = m_parent->getSize() * 0.5f;
  
                const Vec<2, float> position = m_cursor + margin;

                Vec<2, float> offset = { m_size.x * 0.5f, m_size.y * 0.5f };

                if (position.x >= parentCenter.x)
                {
                    offset.x *= -1;
                }

                if (position.y >= parentCenter.y)
                {
                    offset.y *= -1;
                }

                margin.x += offset.x;
                margin.y += offset.y;

                const Vec<2, float> cursor = m_size + margin;

                m_parent->addCursor({ cursor.x, 0.0f });
            }

            setPosition(m_cursor + margin);
        }

        float Component::calculateSize(const std::string& inValue, Style::Direction inDirection) const
        {
            if (inValue.empty())
            {
                return 0.0f;
            }

            if (String::startsWith(inValue, Style::VARIABLE_KEYWORD))
            {
                return calculateSize(parseText(Style::variableToReference(inValue)), inDirection);
            }

            if (String::startsWith(inValue, Style::CALCULATION_KEYWORD))
            {
                return calculateSizeFromCalculation(inValue, inDirection);
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

            if (!String::isNaN(inValue))
            {
                return std::stof(inValue);
            }

            return 0.0f;
        }

        float Component::calculateSizeFromCalculation(
            const std::string& inValue,
            Style::Direction inDirection
        ) const
        {
            if (!String::startsWith(inValue, Style::CALCULATION_KEYWORD))
            {
                return 0.0f;
            }

            const std::string operation = extractParams(inValue);

            std::uint32_t parathesisCount = 0;

            for (std::uint32_t i = 0; i < operation.size(); i++)
            {
                const char character = operation.at(i);

                if (character == FUNCTION_PARAMS_OPENING)
                {
                    parathesisCount++;

                    continue;
                }

                if (character == FUNCTION_PARAMS_CLOSING)
                {
                    parathesisCount--;

                    continue;
                }

                const auto& iterator = std::find(
                    Style::CALCULATION_OPERATORS.begin(),
                    Style::CALCULATION_OPERATORS.end(),
                    character
                );

                if (iterator == Style::CALCULATION_OPERATORS.end() || parathesisCount > 0)
                {
                    continue;
                }

                const float left  = calculateSize(String::trim(operation.substr(0, i)), inDirection);
                const float right = calculateSize(String::trim(operation.substr(i + 1)), inDirection);

                if (character == Style::CALCULATION_OPERATOR_SUM)
                {
                    return left + right;
                }
    
                if (character == Style::CALCULATION_OPERATOR_SUB)
                {
                    return left - right;
                }
    
                if (character == Style::CALCULATION_OPERATOR_MUL)
                {
                    return left * right;
                }
    
                if (character == Style::CALCULATION_OPERATOR_DIV)
                {
                    return left / right;
                }

                break;
            }

            return 0.0f;
        }

        float Component::calculateSizeFromPercentage(
            const std::string& inValue,
            Style::Direction inDirection
        ) const
        {
            if (!String::endsWith(inValue, Style::PERCENTAGE_SIZE_UNIT))
            {
                return 0.0f;
            }

            return calculateSizeFromPercentage(
                std::stof(
                    inValue.substr(
                        0,
                        inValue.length() - strlen(Style::PERCENTAGE_SIZE_UNIT)
                    )
                ),
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

            const Vec<2, float>& size = isPositionStyle(Style::Position::Absolute) ?
                m_root->getSize() :
                m_parent->getSize();

            const Vec<2, float>& offset = isPositionStyle(Style::Position::Absolute) ?
                Vec2Zero :
                m_cursor;

            if (inDirection == Style::Direction::Horizontal)
            {
                return std::max(0.0f, size.x - offset.x) * value;
            }

            return std::max(0.0f, size.y - offset.y) * value;
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
                std::stof(
                    inValue.substr(
                        0,
                        inValue.length() - strlen(Style::VIEWPORT_HEIGHT_SIZE_UNIT)
                    )
                )
            );
        }

        float Component::calculateSizeFromViewportHeight(float inVhValue) const
        {
            return m_root->getSize().y * (inVhValue / 100.0f);
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

            return calculateSizeFromViewportHeight(
                std::stof(
                    inValue.substr(
                        0,
                        inValue.length() - strlen(Style::VIEWPORT_WIDTH_SIZE_UNIT)
                    )
                )
            );
        }

        float Component::calculateSizeFromViewportWidth(float inVwValue) const
        {
            return  m_root->getSize().x * (inVwValue / 100.0f);
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
        
            return std::stof(
                inValue.substr(
                    0,
                    inValue.length() - strlen(Style::PIXEL_SIZE_UNIT)
                )
            );
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

        bool Component::doesTextContainsReference(const std::string& inValue) const
        {
            bool hasOpening = inValue.find_first_of(REFERENCE_VALUE_OPENING) != std::string::npos;
            bool hasClosing = inValue.find_last_of(REFERENCE_VALUE_CLOSING) != std::string::npos;

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

            return getFunction(data.name)(event);
        }

        bool Component::doesTextContainsFunction(const std::string& inValue) const
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

            for (std::string& value : String::split(params, ","))
            {
                data.params.push_back(
                    parseReference(String::trim(value))
                );
            }

            return data;
        }
    }
}