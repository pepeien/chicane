#include "Chicane/Grid/Component.reflected.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"
#include "Chicane/Core/Script/Handle.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

#include "Chicane/Grid/Component/LayoutMetrics.hpp"
#include "Chicane/Grid/Component/Loading.hpp"
#include "Chicane/Grid/Component/Projection.hpp"
#include "Chicane/Grid/Component/Scope.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/StyleMatch.hpp"
#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Grid/Component/Script.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component* Component::sCreate(const XmlNode& inNode)
        {
            if (inNode.empty() || !inNode.isElement() || sIsContentSlot(inNode))
            {
                return nullptr;
            }

            const String              tag  = inNode.getName();
            const ReflectionTypeInfo* type = ReflectionTypeRegistry::sInstance().find(String("Chicane::Grid::") + tag);

            if (!type)
            {
                type = ReflectionTypeRegistry::sInstance().find(tag);
            }

            if (!type && !tag.isEmpty())
            {
                const String pascal = tag.substr(0, 1).toUpper() + tag.substr(1);

                if (!pascal.equals(tag))
                {
                    type = ReflectionTypeRegistry::sInstance().find(String("Chicane::Grid::") + pascal);
                }
            }

            if (!type && Scope::sCurrent())
            {
                type = Scope::sCurrent()->findImported(tag);
            }

            if (!type)
            {
                return nullptr;
            }

            Component* instance = type->create<Component>({inNode});
            if (instance)
            {
                instance->m_importOwner = Scope::sCurrent();
            }

            return instance;
        }

        Component* Component::sCreate(const FileSystem::Path& inTemplate)
        {
            if (inTemplate.isEmpty())
            {
                return nullptr;
            }

            const XmlDocument document = Xml::load(inTemplate);
            if (document.empty())
            {
                return nullptr;
            }

            Component* created = sCreate(document.getFirstChild());
            if (!created)
            {
                return nullptr;
            }

            created->loadScript(inTemplate);

            return created;
        }

        bool Component::sIsContentSlot(const XmlNode& inNode)
        {
            if (inNode.empty() || !inNode.isElement())
            {
                return false;
            }

            return String(inNode.getName()).equals(CONTENT_TAG_ID);
        }

        Component::Component(const XmlNode& inNode)
            : Component(inNode.getName())
        {
            parse(inNode);
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            addChildren(getSource());

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
                        const bool bDisplayEmpty = static_cast<bool>(style.display.getRaw().isEmpty());

                        if (bDisplayEmpty)
                        {
                            style.display.set(StyleDisplay::Block);
                        }

                        if (!bDisplayEmpty)
                        {
                            style.display.refresh();
                        }

                        return;
                    }

                    style.display.set(StyleDisplay::None);
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

                    const std::vector<String> values = inValue.trim().split(':');
                    if (values.size() < 2)
                    {
                        return;
                    }

                    const String variableId = values.at(0).trim();
                    const String accessorId = values.at(1).trim();

                    Component* owner = hasParent() ? getParent() : this;
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
            : Animatable(),
              style({}),
              classList(),
              m_tag(inTag),
              m_id(String::sEmpty()),
              m_classListSubscription({}),
              m_status(ComponentStatus::None),
              m_flags(ComponentDirty::Style | ComponentDirty::Layout),
              m_bDescendantPaintDirty(false),
              m_bCommandsDirty(false),
              m_live(String::sEmpty()),
              m_liveHash(0),
              m_directives({}),
              m_variables({}),
              m_styleVariables({}),
              m_styleFile(nullptr),
              m_styles(nullptr),
              m_bHasOwnStyle(false),
              m_imports({}),
              m_importOwner(nullptr),
              m_root(nullptr),
              m_parent(nullptr),
              m_children({}),
              m_peripherals({}),
              m_paintChildren({}),
              m_size(Vec2::sZero()),
              m_scale(Vec2::sZero()),
              m_offset(Vec2::sZero()),
              m_cursor(Vec2::sZero()),
              m_scratch(0.0f),
              m_layoutParentSize(Vec2(-1.0f)),
              m_layoutParentFontSize(-1.0f),
              m_primitive({}),
              m_draw({}),
              m_forInstances({}),
              m_forVariable(String::sEmpty()),
              m_forSource({}),
              m_script(nullptr)
        {
            style.setParent(this);

            classList.watchChanges([this]() { onClassListChanged(); });

            Script::Handle::add(this);
        }

        Component::~Component()
        {
            Script::Handle::remove(this);

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
            if (m_id.equals(inValue))
            {
                return;
            }

            m_id = inValue;

            markStyleDirty();
            markLayoutDirty();
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

            XmlDocument document = Xml::load(inTemplate);
            if (document.empty())
            {
                throw std::runtime_error("UI document " + source + " does not have any components");
            }

            const XmlNode root = document.getFirstChild();
            if (!(root.getParent() == root.getRoot() && root.getNextSibling().isEmpty()))
            {
                throw std::runtime_error("UI document root element must not have any siblings");
            }

            std::vector<Component*> projected = m_children;
            m_children.clear();

            Scope      scope(this);
            Projection projection(projected);

            {
                Component* wrapper = sCreate(root);

                const bool bNameMatchesTag = static_cast<bool>(String(root.getName()).equals(getTag()));

                if (bNameMatchesTag)
                {
                    parse(root);
                    setId(getAttribute(ID_ATTRIBUTE_NAME));
                    setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));
                    addChildren(getSource());
                }

                const bool bHasWrapper = !bNameMatchesTag && (wrapper);

                if (bHasWrapper)
                {
                    addChild(wrapper);
                }
            }

            for (Component* child : projected)
            {
                addChild(child);
            }

            loadScript(inTemplate);
        }

        void Component::loadScript(const FileSystem::Path& inTemplate)
        {
            m_script.reset();

            if (inTemplate.isEmpty())
            {
                return;
            }

            const FileSystem::Path script = inTemplate.withExtension(ComponentScript::EXTENSION);
            if (!FileSystem::exists(script))
            {
                return;
            }

            m_script = std::make_unique<ComponentScript>(this);
            if (!m_script->load(script))
            {
                m_script.reset();
            }
        }

        bool Component::callScript(const String& inName, const std::vector<String>& inArgs) const
        {
            return m_script && m_script->callGlobal(inName, inArgs);
        }

        void Component::releaseScript()
        {
            m_script.reset();
        }

        void Component::addProjectedContent(const XmlNode& inSlot)
        {
            std::vector<Component*>* projected = Projection::sCurrent();
            if (!projected || projected->empty())
            {
                return;
            }

            const String select = inSlot.parseString(CONTENT_SELECT_ATTRIBUTE_NAME, String::sEmpty()).trim();

            std::vector<Component*> leftover;
            leftover.reserve(projected->size());

            for (Component* child : *projected)
            {
                if (!child)
                {
                    continue;
                }

                if (select.isEmpty() || child->hasLocalSelector(select))
                {
                    addChild(child);

                    continue;
                }

                leftover.push_back(child);
            }

            *projected = std::move(leftover);
        }

        Component* Component::cloneTemplate() const
        {
            if (m_source.empty())
            {
                return nullptr;
            }

            Scope      scope(m_importOwner);
            Component* clone = sCreate(m_source);
            if (!clone)
            {
                return nullptr;
            }

            clone->removeAttribute(FOR_DIRECTIVE_KEYWORD);

            if (clone->classList.isEmpty() && !classList.isEmpty())
            {
                clone->setClassList(classList);
            }

            return clone;
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
    }
}
