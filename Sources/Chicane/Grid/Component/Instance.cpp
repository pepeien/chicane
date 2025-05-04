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

        Component::Component(const pugi::xml_node& inNode)
            : Component(inNode.name(), XML::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string())
        {}

        Component::Component(const std::string& inTag, const std::string& inId)
            : m_tag(inTag),
            m_id(inId),
            m_class(""),
            m_style({}),
            m_functions({}),
            m_root(nullptr),
            m_parent(nullptr),
            m_children({})
        {}

        Component::~Component()
        {
            for (Component* child : m_children)
            {
                delete child;
            }

            m_children.clear();
        }

        bool Component::isValidChild(const Component* inComponent) const
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

        const Vec<2, int>& Component::getSize() const
        {
            return m_size;
        }

        void Component::setSize(const Vec<2, int>& inSize)
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
        }

        const Vec<2, int>& Component::getPosition() const
        {
            return m_position;
        }

        void Component::setPosition(const Vec<2, int>& inPosition)
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

        const Component* Component::getRoot() const
        {
            return m_root;
        }

        void Component::setRoot(const Component* inComponent)
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

        const Component* Component::getParent() const
        {
            return m_parent;
        }

        void Component::setParent(const Component* inComponent)
        {
            if (inComponent == this)
            {
                return;
            }

            m_parent = inComponent;
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

                addChild(component);

                component->addChildren(child);
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
    }
}