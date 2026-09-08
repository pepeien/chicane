#include "Chicane/Grid/Component/Viewport.reflected.hpp"

#include <algorithm>

#include "Chicane/Grid/Component/Dock/Panel.hpp"

namespace Chicane
{
    namespace Grid
    {
        Viewport::Viewport(const pugi::xml_node& inNode)
            : Container(inNode),
              target(String::empty())
        {}

        void Viewport::onRefresh()
        {
            Container::onRefresh();

            refreshTarget();
        }

        void Viewport::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            if (!isReference(getAttribute(TARGET_ATTRIBUTE_NAME)) && !target.isEmpty() && !m_bIsLaidOutThisFrame)
            {
                return;
            }

            refreshTarget();
        }

        void Viewport::refreshSize()
        {
            if (hasParent() && m_parent->getTag().equals(DockPanel::TAG_ID))
            {
                const Vec2 inner = m_parent->getInnerLayoutSize();
                setSize(std::max(0.0f, inner.x), std::max(0.0f, inner.y));

                return;
            }

            Component::refreshSize();
        }

        void Viewport::refreshPosition()
        {
            if (hasParent() && m_parent->getTag().equals(DockPanel::TAG_ID))
            {
                const Style& parentStyle = m_parent->getStyle();
                setPosition(
                    m_parent->getPosition().x + parentStyle.insetLeft(),
                    m_parent->getPosition().y + parentStyle.insetTop()
                );
                addCursor(m_style.insetLeft(), m_style.insetTop());

                return;
            }

            Component::refreshPosition();
        }

        void Viewport::refreshTarget()
        {
            target = parseText(getAttribute(TARGET_ATTRIBUTE_NAME)).trim();

            if (target.isEmpty())
            {
                target = DEFAULT_TARGET;
            }

            m_style.background.image.setRaw(target);
        }
    }
}
