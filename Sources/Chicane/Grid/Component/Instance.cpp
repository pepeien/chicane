#include "Grid/Component/Instance.hpp"

#include "Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        Component::Component(const pugi::xml_node& inNode)
            : m_id(XML::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string()),
            m_style(Style(inNode)),
            m_functions({}),
            m_children({})
        {}

        Component::Component(const std::string& inId)
            : m_id(inId),
            m_style(Style()),
            m_functions({}),
            m_children({})
        {}

        void Component::render() const
        {
            if (m_style.display == Style::Display::None) {
                return;
            }

            for (const Component* child : m_children)
            {
                child->render();
            }
        }

        const std::string& Component::getId() const
        {
            return m_id;
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

        bool Component::hasParent() const
        {
            return m_parent != nullptr;
        }

        const Component* Component::getParent() const
        {
            return m_parent;
        }

        void Component::setParent(const Component* inParent)
        {
            if (inParent == this)
            {
                return;
            }

            m_parent = inParent;
        }

        bool Component::hasChildren() const
        {
            return !m_children.empty();
        }

        const std::vector<Component*>& Component::getChildren() const
        {
            return m_children;
        }

        void Component::addChild(Component* inChild)
        {
            if (inChild == nullptr || inChild == this)
            {
                return;
            }

            inChild->setParent(this);

            m_children.push_back(inChild);
        }
    }
}