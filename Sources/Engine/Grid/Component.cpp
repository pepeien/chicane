#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>

#include "Chicane/Core/Reflection/Type/Registry.hpp"

#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/View.hpp"

namespace Chicane
{
    namespace Grid
    {
        thread_local Component* g_scope = nullptr;

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
            if (inNode.empty() || inNode.type() != pugi::node_element)
            {
                return nullptr;
            }

            const String              tag = inNode.name();
            const ReflectionTypeInfo* type =
                ReflectionTypeRegistry::getInstance().find(String("Chicane::Grid::") + tag);

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

        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name())
        {
            m_sourceNode = m_sourceDocument.append_copy(inNode);
            m_attributes = Xml::getAttributes(m_sourceNode);
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

                    Component* owner = this;
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
            : Transformable2D(),
              m_tag(inTag),
              m_id(String::empty()),
              m_className(String::empty()),
              m_directives({}),
              m_variables({}),
              m_style({}),
              m_styleBase({}),
              m_bHasStyleBase(false),
              m_styleVariables({}),
              m_styleFile(nullptr),
              m_styles(nullptr),
              m_bOwnsStyle(false),
              m_imports({}),
              m_importOwner(nullptr),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_size(Vec2::Zero()),
              m_scale(Vec2::Zero()),
              m_offset(Vec2::Zero()),
              m_cursor(Vec2::Zero()),
              m_scratch(0.0f),
              m_primitive({}),
              m_attributes({}),
              m_sourceNode(),
              m_forInstances({}),
              m_forVariable(String::empty()),
              m_forSource({}),
              m_bSkipForDirective(false),
              m_bHovered(false),
              m_bFocused(false)
        {
            m_style.setParent(this);
            m_styleBase.setParent(this);
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

        const ReflectionTypeInfo* Component::findImported(const String& inSelector) const
        {
            if (inSelector.isEmpty())
            {
                return nullptr;
            }

            const auto found = m_imports.find(inSelector);

            return found != m_imports.end() ? found->second : nullptr;
        }

        bool Component::isDrawable() const
        {
            return isDisplayable() && isVisible() && isSolid() && hasPrimitive();
        }

        void Component::tick(float inDeltaTime)
        {
            refresh();

            onTick(inDeltaTime);

            for (std::size_t i = 0; i < m_children.size(); i++)
            {
                m_children.at(i)->tick(inDeltaTime);
            }

            const bool bIsHeightAuto = m_style.height.getRaw().isEmpty() || m_style.height.isRaw(Size::AUTO_KEYWORD);

            addSize(
                m_style.padding.left.get() + m_style.padding.right.get(),
                bIsHeightAuto ? (m_style.padding.top.get() + m_style.padding.bottom.get()) : 0.0f
            );
        }

        void Component::refresh()
        {
            refreshClassName();
            refreshStyle();

            onRefresh();

            refreshSize();
            refreshPosition();
            refreshBounds();

            refreshDirectives();
        }

        void Component::refreshStyleRuleset()
        {
            m_styleVariables.clear();

            if (m_bHasStyleBase)
            {
                m_style.copyValuesFrom(m_styleBase);
            }

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

                    for (const String& selector : source.selectors)
                    {
                        if (!hasSelector(selector.trim()))
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

                        break;
                    }
                }
            }

            if (!properties.empty())
            {
                addStyleProperties(properties);
            }

            if (!m_bHovered && !m_bFocused)
            {
                m_styleBase.copyValuesFrom(m_style);
                m_bHasStyleBase = true;
            }
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

            return (bIsBackgroundImageVisible || bIsBackgroundColorVisible || bIsBackdropVisible) &&
                   m_style.opacity.get() > 0.0f;
        }

        bool Component::isSolid() const
        {
            return m_size.x > 0.0f && m_size.y > 0.0f;
        }

        bool Component::isHovered() const
        {
            return m_bHovered;
        }

        bool Component::isFocused() const
        {
            return m_bFocused;
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

        void Component::setHovered(bool inValue)
        {
            if (m_bHovered == inValue)
            {
                return;
            }

            m_bHovered = inValue;

            refreshStyleRuleset();

            if (inValue)
            {
                hover();

                return;
            }

            leave();
        }

        void Component::setFocused(bool inValue)
        {
            if (m_bFocused == inValue)
            {
                return;
            }

            m_bFocused = inValue;

            refreshStyleRuleset();

            if (inValue)
            {
                focus();

                return;
            }

            blur();
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

        const Style& Component::getStyle() const
        {
            return m_style;
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

        bool Component::hasLocalSelector(const String& inValue) const
        {
            if (inValue.isEmpty())
            {
                return false;
            }

            String value  = inValue.trim();
            bool   bHover = false;
            bool   bFocus = false;

            while (true)
            {
                const std::size_t hoverAt = value.find(Style::PSEUDO_CLASS_HOVER);
                const std::size_t focusAt = value.find(Style::PSEUDO_CLASS_FOCUS);

                if (hoverAt == String::npos && focusAt == String::npos)
                {
                    break;
                }

                if (hoverAt != String::npos && (focusAt == String::npos || hoverAt <= focusAt))
                {
                    bHover = true;
                    value  = value.substr(0, hoverAt) + value.substr(hoverAt + std::strlen(Style::PSEUDO_CLASS_HOVER));

                    continue;
                }

                bFocus = true;
                value  = value.substr(0, focusAt) + value.substr(focusAt + std::strlen(Style::PSEUDO_CLASS_FOCUS));
            }

            if (bHover && !m_bHovered)
            {
                return false;
            }

            if (bFocus && !m_bFocused)
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

            Scope scope(this);

            if (String(root.name()).equals(getTag()))
            {
                m_sourceNode = m_sourceDocument.append_copy(root);
                m_attributes = Xml::getAttributes(m_sourceNode);
                setId(getAttribute(ID_ATTRIBUTE_NAME));
                setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));
                addChildren(root);

                return;
            }

            std::vector<Component*> projected = m_children;
            m_children.clear();

            if (Component* wrapper = create(root))
            {
                addChild(wrapper);
            }

            for (Component* child : projected)
            {
                addChild(child);
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

            for (const String& rawParam : splitMethodParams(paramsRaw))
            {
                const String param = rawParam.trim();
                if (param.isEmpty())
                {
                    continue;
                }

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

        std::vector<Component*> Component::getChildrenFlat() const
        {
            std::vector<Component*>       result;
            const std::vector<Component*> children = m_children;

            for (Component* child : children)
            {
                if (!child || child == this)
                {
                    continue;
                }

                result.push_back(child);

                std::vector<Component*> sub = child->getChildrenFlat();
                result.insert(result.end(), sub.begin(), sub.end());
            }

            return result;
        }

        Component* Component::getHitAt(const Vec2& inLocation) const
        {
            Component* hit = nullptr;

            for (Component* child : getChildrenFlat())
            {
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
            const Bounds2D box = getDrawBounds();
            if (!box.containsRounded(inLocation, m_style.radius.horizontal(), m_style.radius.vertical()))
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
                if (const Scrollable* scrollable = dynamic_cast<const Scrollable*>(ancestor))
                {
                    if (scrollable->clipsOverflow() && !ancestor->getDrawBounds().containsRounded(
                                                           inLocation,
                                                           ancestor->getStyle().radius.horizontal(),
                                                           ancestor->getStyle().radius.vertical()
                                                       ))
                    {
                        return false;
                    }
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

        bool Component::onEvent(const WindowEvent&)
        {
            return false;
        }

        void Component::addChildren(const pugi::xml_node& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            for (const auto& child : inNode.children())
            {
                Component* component = create(child);
                if (!component)
                {
                    continue;
                }

                addChild(component);
            }
        }

        void Component::addChild(Component* inComponent, std::size_t inIndex)
        {
            if (!canAdopt(inComponent))
            {
                return;
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
        }

        Vec2 Component::getChildrenContentSizeBlock() const
        {
            Vec2 result = Vec2::Zero();

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style = child->getStyle();

                const Vec2 margin = {
                    (style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.left.get()) +
                        (style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get()),
                    (style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.top.get()) +
                        (style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get())
                };

                const Vec2 occupied = {
                    (child->getPosition().x - getPosition().x) + child->getSize().x + margin.x,
                    (child->getPosition().y - getPosition().y) + child->getSize().y + margin.y
                };

                result.x = std::max(result.x, occupied.x);
                result.y = std::max(result.y, occupied.y);
            }

            return result;
        }

        Vec2 Component::getChildrenContentSizeFlex() const
        {
            Vec2 result = Vec2::Zero();

            for (const Component* child : m_children)
            {
                if (!child || !child->isDisplayable() || child->getStyle().isPosition(StylePosition::Absolute))
                {
                    continue;
                }

                const Style& style = child->getStyle();

                const Vec2 margin = {
                    style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.right.get(),
                    style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : style.margin.bottom.get()
                };

                result.x =
                    std::max(result.x, (child->getPosition().x - getPosition().x) + child->getSize().x + margin.x);
                result.y =
                    std::max(result.y, (child->getPosition().y - getPosition().y) + child->getSize().y + margin.y);
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
            Vec2 result = getTranslation();

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                if (const Scrollable* scrollable = dynamic_cast<const Scrollable*>(ancestor))
                {
                    result.x -= scrollable->getScroll().x;
                    result.y -= scrollable->getScroll().y;
                }

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return result;
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
            Bounds2D result;

            const Vec2 position = getDrawPosition();

            result.left   = position.x;
            result.top    = position.y;
            result.right  = position.x + m_size.x;
            result.bottom = position.y + m_size.y;

            return result;
        }

        Bounds2D Component::getOverflowClip() const
        {
            Bounds2D clip = Bounds2D::unconstrained();

            const Component* ancestor = m_parent;
            while (ancestor && ancestor != this)
            {
                if (const Scrollable* scrollable = dynamic_cast<const Scrollable*>(ancestor))
                {
                    if (scrollable->clipsOverflow())
                    {
                        clip = clip.intersect(ancestor->getDrawBounds());
                    }
                }

                if (ancestor->isRoot())
                {
                    break;
                }

                ancestor = ancestor->getParent();
            }

            return clip;
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
                if (const Scrollable* scrollable = dynamic_cast<const Scrollable*>(ancestor))
                {
                    if (scrollable->clipsOverflow() && !ancestor->getStyle().radius.isZero())
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

        void Component::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            const bool bIsWidthAuto  = m_style.width.getRaw().isEmpty() || m_style.width.isRaw(Size::AUTO_KEYWORD);
            const bool bIsHeightAuto = m_style.height.getRaw().isEmpty() || m_style.height.isRaw(Size::AUTO_KEYWORD);

            float width  = m_style.width.get();
            float height = m_style.height.get();

            if (bIsWidthAuto || bIsHeightAuto)
            {
                const Vec2 content = getChildrenContentSize();

                if (bIsWidthAuto)
                {
                    const bool bIsFlexRowItem = hasParent() && m_parent->getStyle().isDisplay(StyleDisplay::Flex) &&
                                                m_parent->getStyle().flex.direction.get() == StyleFlexDirection::Row &&
                                                !m_style.isPosition(StylePosition::Absolute);

                    if (bIsFlexRowItem || !hasParent())
                    {
                        width = content.x;
                    }
                    else
                    {
                        const Style& parentStyle = m_parent->getStyle();
                        const float  available =
                            m_parent->getSize().x - parentStyle.padding.left.get() - parentStyle.padding.right.get();
                        const float horizontalMargin =
                            (m_style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.left.get()) +
                            (m_style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.right.get());

                        width = std::max(
                            0.0f,
                            available - horizontalMargin - m_style.padding.left.get() - m_style.padding.right.get()
                        );
                    }
                }

                if (bIsHeightAuto)
                {
                    height = content.y;
                }
            }

            setSize(width, height);
        }

        void Component::refreshPosition()
        {
            setPosition(0.0f, 0.0f);

            float marginLeft   = m_style.margin.left.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.left.get();
            float marginRight  = m_style.margin.right.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.right.get();
            float marginTop    = m_style.margin.top.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.top.get();
            float marginBottom = m_style.margin.bottom.isRaw(Size::AUTO_KEYWORD) ? 0.0f : m_style.margin.bottom.get();

            const Vec2 startPadding(m_style.padding.left.get(), m_style.padding.top.get());

            const bool bIsHeightAuto = m_style.height.getRaw().isEmpty() || m_style.height.isRaw(Size::AUTO_KEYWORD);

            const float usedWidth = m_size.x + m_style.padding.left.get() + m_style.padding.right.get();
            const float usedHeight =
                m_size.y + (bIsHeightAuto ? (m_style.padding.top.get() + m_style.padding.bottom.get()) : 0.0f);

            if (hasParent() && !isRoot())
            {
                const Style& parentStyle = m_parent->getStyle();
                const Vec2   available   = {
                    std::max(
                        0.0f,
                        m_parent->getSize().x - parentStyle.padding.left.get() - parentStyle.padding.right.get()
                    ),
                    std::max(
                        0.0f,
                        m_parent->getSize().y - parentStyle.padding.top.get() - parentStyle.padding.bottom.get()
                    )
                };

                const bool bLeftAuto   = m_style.margin.left.isRaw(Size::AUTO_KEYWORD);
                const bool bRightAuto  = m_style.margin.right.isRaw(Size::AUTO_KEYWORD);
                const bool bTopAuto    = m_style.margin.top.isRaw(Size::AUTO_KEYWORD);
                const bool bBottomAuto = m_style.margin.bottom.isRaw(Size::AUTO_KEYWORD);

                const float leftoverW = available.x - usedWidth - marginLeft - marginRight;
                if (leftoverW > 0.0f)
                {
                    if (bLeftAuto && bRightAuto)
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

                const bool bCanAutoVertical =
                    m_style.isPosition(StylePosition::Absolute) || parentStyle.isDisplay(StyleDisplay::Flex);

                if (bCanAutoVertical && (bTopAuto || bBottomAuto))
                {
                    float leftoverH = available.y - usedHeight - marginTop - marginBottom;

                    if (!m_style.isPosition(StylePosition::Absolute) && parentStyle.isDisplay(StyleDisplay::Flex) &&
                        parentStyle.flex.direction.get() == StyleFlexDirection::Column)
                    {
                        leftoverH = (m_parent->getPosition().y + parentStyle.padding.top.get() + available.y) -
                                    m_parent->getCursor().y - usedHeight - marginBottom;
                    }

                    if (leftoverH > 0.0f)
                    {
                        if (bTopAuto && bBottomAuto)
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
                setPosition(marginLeft, marginTop);
                addCursor(startPadding);

                return;
            }

            const Style& parentStyle = m_parent->getStyle();
            const Vec2   available   = {
                std::max(
                    0.0f,
                    m_parent->getSize().x - parentStyle.padding.left.get() - parentStyle.padding.right.get()
                ),
                std::max(0.0f, m_parent->getSize().y - parentStyle.padding.top.get() - parentStyle.padding.bottom.get())
            };

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

                const float lineStart    = bIsRow ? (m_parent->getPosition().x + parentStyle.padding.left.get())
                                                  : (m_parent->getPosition().y + parentStyle.padding.top.get());
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

        void Component::refreshBounds()
        {
            m_bounds.set(0.0f, 0.0f, m_size.y, m_size.x);
            m_bounds.transform(getMatrix());
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
                String result = parseMethod(inValue);

                if (result.isEmpty())
                {
                    return result;
                }

                return hasParent() ? m_parent->parseReference(inValue) : inValue;
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

            const std::size_t         dot      = qualified.lastOf('.');
            const String              receiver = dot == String::npos ? String::empty() : qualified.substr(0, dot);
            const String              name     = dot == String::npos ? qualified : qualified.substr(dot + 1);
            const ReflectionTypeInfo* type     = nullptr;
            void*                     instance = nullptr;

            if (receiver.isEmpty())
            {
                type     = ReflectionTypeRegistry::getInstance().find(typeid(*this));
                instance = const_cast<Component*>(this);
            }
            else
            {
                const ReflectionFieldAccessor accessor = getField(receiver);

                if (!accessor.isValid() || !accessor.typeIndex.has_value())
                {
                    return String::empty();
                }

                type     = ReflectionTypeRegistry::getInstance().find(accessor.typeIndex.value());
                instance = const_cast<char*>(accessor.address(this));
            }

            if (!type || !instance || name.isEmpty())
            {
                return String::empty();
            }

            const ReflectionTypeMethodInfo* method = type->findMethod(name);
            if (!method)
            {
                return String::empty();
            }

            return method->toString(method->invoke(instance));
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
                instance->m_style.display.set(StyleDisplay::Flex);
                instance->refreshStyleRuleset();
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
    }
}