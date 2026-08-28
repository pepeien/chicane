#include "Chicane/Grid/Component/Viewport.reflected.hpp"

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

            refreshTarget();
        }

        void Viewport::refreshTarget()
        {
            m_style.background.image.setRaw(parseText(getAttribute(TARGET_ATTRIBUTE_NAME)).trim());
        }
    }
}
