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
        bool Component::isRoot() const
        {
            return (!m_parent && !m_root) || (m_parent == this && m_root == this);
        }

        bool Component::canAdopt(Component* inComponent) const
        {
            if (!inComponent || inComponent == this)
            {
                return false;
            }

            for (const Component* ancestor = this; ancestor != nullptr;
                 ancestor                  = ancestor->isRoot() ? nullptr : ancestor->m_parent)
            {
                if (ancestor == inComponent)
                {
                    return false;
                }
            }

            return true;
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

        const Component* Component::getContainingBlock() const
        {
            if (isRoot() || !hasParent())
            {
                return this;
            }

            if (!style.isPosition(StylePosition::Absolute))
            {
                return m_parent;
            }

            for (const Component* ancestor = m_parent; ancestor != nullptr; ancestor = ancestor->getParent())
            {
                if (ancestor->isRoot() || ancestor->getStyle().isPositioned())
                {
                    return ancestor;
                }
            }

            return hasRoot() ? m_root : this;
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

        const std::vector<Component*>& Component::getPaintChildren() const
        {
            return m_paintChildren;
        }

        std::vector<Component*> Component::getChildrenFlat() const
        {
            std::vector<Component*> result;
            appendChildrenFlat(result);

            return result;
        }

        void Component::appendChildrenFlat(std::vector<Component*>& outChildren) const
        {
            std::unordered_set<const Component*> visited;
            visited.insert(this);

            std::vector<const Component*> stack;
            stack.push_back(this);

            while (!stack.empty())
            {
                const Component* node = stack.back();
                stack.pop_back();

                const std::vector<Component*> children = node->m_paintChildren;
                std::vector<Component*>       next;
                next.reserve(children.size());

                for (Component* child : children)
                {
                    if (!child || !visited.insert(child).second)
                    {
                        continue;
                    }

                    outChildren.push_back(child);
                    next.push_back(child);
                }

                for (auto it = next.rbegin(); it != next.rend(); it++)
                {
                    stack.push_back(*it);
                }
            }
        }

        void Component::setPeripherals(const std::vector<Component*>& inPeripherals)
        {
            if (m_peripherals == inPeripherals)
            {
                return;
            }

            m_peripherals = inPeripherals;

            rebuildPaintChildren();
        }

        Vec2 Component::getScrollOffset() const
        {
            return Vec2::sZero();
        }

        Vec2 Component::getScrollBarGutter() const
        {
            return Vec2::sZero();
        }

        bool Component::containsNode(const Component* inNode) const
        {
            for (const Component* node = inNode; node != nullptr; node = node->getParent())
            {
                if (node == this)
                {
                    return true;
                }

                if (node->isRoot())
                {
                    break;
                }
            }

            return false;
        }

        bool Component::broadcastEvent(const WindowEvent& inEvent)
        {
            std::vector<Component*> stack;
            stack.reserve(m_children.size());

            for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
            {
                if (*it)
                {
                    stack.push_back(*it);
                }
            }

            while (!stack.empty())
            {
                Component* child = stack.back();
                stack.pop_back();

                if (child->isCulled())
                {
                    continue;
                }

                if (child->onEvent(inEvent))
                {
                    return true;
                }

                for (auto it = child->m_children.rbegin(); it != child->m_children.rend(); it++)
                {
                    if (*it)
                    {
                        stack.push_back(*it);
                    }
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

        void Component::addChildren(const XmlNode& inNode)
        {
            if (inNode.empty())
            {
                return;
            }

            bool bWasAdopted = false;
            for (const XmlNode& child : inNode.getChildren())
            {
                if (sIsContentSlot(child))
                {
                    addProjectedContent(child);

                    continue;
                }

                Component* component = sCreate(child);
                if (!component)
                {
                    continue;
                }

                if (adoptChild(component))
                {
                    bWasAdopted = true;
                }
            }

            if (bWasAdopted)
            {
                rebuildPaintChildren();
                markLayoutDirty();
            }
        }

        bool Component::adoptChild(Component* inComponent, std::size_t inIndex)
        {
            if (!canAdopt(inComponent))
            {
                return false;
            }

            inComponent->setRoot(m_root);
            inComponent->setParent(this);
            inComponent->setStyleFile(inComponent->m_bHasOwnStyle ? inComponent->m_styleFile : m_styleFile);

            const bool bIndexSize = static_cast<bool>(inIndex >= m_children.size());

            if (bIndexSize)
            {
                m_children.push_back(inComponent);
            }

            if (!bIndexSize)
            {
                m_children.insert(m_children.begin() + static_cast<std::ptrdiff_t>(inIndex), inComponent);
            }

            onAdopted(inComponent);

            return true;
        }

        void Component::addChild(Component* inComponent, std::size_t inIndex)
        {
            if (!adoptChild(inComponent, inIndex))
            {
                return;
            }

            rebuildPaintChildren();
            markLayoutDirty();
        }

        void Component::releaseChild(Component* inComponent)
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

            rebuildPaintChildren();
            markLayoutDirty();
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
            rebuildPaintChildren();
            markLayoutDirty();
        }
    }
}
