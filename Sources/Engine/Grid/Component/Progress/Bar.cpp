#include "Chicane/Grid/Component/Progress/Bar.reflected.hpp"

namespace Chicane
{
    namespace Grid
    {
        ProgressBar::ProgressBar(const pugi::xml_node& inNode)
            : Container(inNode),
              percentage(0.0f)
        {
            load("Assets/Engine/UI/Components/Progress/Bar.grid", "Assets/Engine/UI/Components/Progress/Bar.decal");
        }

        void ProgressBar::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            if (!isReference(getAttribute(PERCENTAGE_ATTRIBUTE_NAME)) && !m_bIsLaidOutThisFrame)
            {
                return;
            }

            refreshPercentage();
        }

        void ProgressBar::refreshPercentage()
        {
            const String value = parseText(getAttribute(PERCENTAGE_ATTRIBUTE_NAME)).trim();
            if (value.isEmpty())
            {
                percentage = 0.0f;

                return;
            }

            percentage = std::clamp(static_cast<float>(std::strtod(value.toChar(), nullptr)), 0.0f, 100.0f);
        }
    }
}